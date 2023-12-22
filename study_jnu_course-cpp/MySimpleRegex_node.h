﻿#ifndef __MY_SIMPLE_REGEX_NODE_H__
#define __MY_SIMPLE_REGEX_NODE_H__
#include "MySimpleRegex.h"


/*********************************/
/*    class compiled's           */
/*    Inner Classes & members    */
/*********************************/
namespace assignment1 {


//상태 머신의 노드를 표현하는 클래스
class compiled::node {
    friend class node_ptr;
private:
    string m_name;              // state node name
    size_t m_state_istart;    // state가 첫 active되었을 때 입력되었던 문자의 인덱스
    bool m_state_active;        // 0: deactive, 1: active
    bool m_is_terminal;         // 0: terminal, 1: non-terminal

    vector<node_ptr*> m_next;     // next link
    vector<node_ptr*> m_reverse_ref;  //이 노드를 가리키는 노드 포인터 리스트(얕은 복사됨)

public:
    node(string name, bool isTerminal = false);
    virtual ~node();

    /*********  조회 함수  *********/
    const string& name() const;
    size_t index_start() const;   //is_accepted일 때 인식된 문자열의 첫 위치를 의미한다.  
    bool is_accepted() const;       //본 노드에서 accept 되었는지 확인한다.
    const vector<node_ptr*>& next() const;      //next 목록을 반환한다.
    const vector<node_ptr*>& reverse_ref() const;   //리버스 참조목록을 반환한다.

    /*********  변경 함수  *********/
    void add_link(node_ptr* _next); //본 노드가 가리키는 노드를 추가한다.
    void delete_link(node* _target, char _ch = '\0'); //타겟을 가리키는 링크를 삭제한다.
    void active(size_t state_istart); //노드를 활성화 상태로 만든다.
    void clear_flags(); //노드 상태와 관련된 값과 플래그들을 모두 클리어한다.
    void set_terminal(bool is_terminal);


    //노드에 한 문자를 입력한다. 
    //노드가 활성화됐을 때 문자가 매치되면 next 노드들에게 활성화를 요청한다.
    //노드 활성화 && 터미널 노드인 경우 is_accepted가 true가 된다.
    void input(vector<active_request_info>& next_active, const char ch);
};


//node를 가리키는 특별한 포인터 추상 클래스
class compiled::node_ptr {
    friend class node;
protected:
    node* m_origin; //타겟을 하는 주체 노드
    node* m_target; //타겟 당하는 피 노드
    Imatchable* matcher;

    //각 포인터가 수행할 전이 동작을 수행한다. 오버라이딩 필요
    virtual void transition_action(
        vector<active_request_info>& next_active, size_t state_istart) = 0;

    //포인터와 관련된 플래그를 클리어한다. 필요한 경우만 오버라이드한다.
    virtual void clear_flag() {}


public:
    static const size_t INF = ~0ULL; //2^32 - 1

    node_ptr(node* pOrigin, Imatchable* ranged_string, node* pNode) 
        : m_origin(pOrigin), m_target(pNode), matcher(ranged_string) {}
    virtual ~node_ptr() {
        if (matcher != nullptr)
            delete matcher; //동적 할당된 matcher를 반환한다.
    }

    //node*로의 명시적 형변환
    explicit operator node*() { return m_target; }

    //현재 포인터를 새로운 연결을 가진 포인터로 복사한다.
    virtual node_ptr* copy(node* new_pNode) = 0;

    //연결된 노드가 다음 사이클에 active 되도록 요청한다.
    //한번에 될 수도 있고, 그렇지 않을 수도 있음
    virtual void request_active_nexttime(
        vector<active_request_info>& next_active, 
        size_t state_istart, char ch ) final 
    { 
        if (matcher->test(ch)) //요구되는 문자와 일치되는 경우에만 전이 행동을 취한다.
            transition_action(next_active, state_istart);
    }



    /*********  조회 함수  *********/
public:
    bool equals(const node_ptr* rhs) {
        if (this == rhs) return true;

        bool pred = m_origin == rhs->m_origin;
        pred &= m_target == rhs->m_target;
        pred &= matcher  == rhs->matcher;

        return pred;
    }

    //matcher에 문자를 넣은 결과를 반환한다.
    bool test(char _ch) const {
        return matcher->test(_ch);
    }

    //타겟을 하는 주체 노드를 가져온다.
    const node* origin() const {
        return m_origin;
    }

    //is_accepted일 때 인식된 문자열의 첫 위치를 의미한다.
    size_t index_start() const {
        return m_target->index_start();
    }

    //본 노드에서 accept 되었는지 확인한다.
    bool is_accepted() const {
        return m_target->is_accepted();
    }

    //노드 포인터가 가리키는 노드와 파라미터 노드가 같은지 확인한다.
    bool is_ref_equal(const node* pNode) const {
        return m_target == pNode;
    }
};


//활성화 신호를 바로 전달하는 노드 포인터
class compiled::node_ptr_direct : public node_ptr {
protected:
    virtual void transition_action(vector<active_request_info>& next_active, size_t state_istart) override {
        next_active.push_back({ m_target, state_istart });
    }

public:
    node_ptr_direct(node* pOrigin, Imatchable* matcher, node* pNode) 
        : node_ptr(pOrigin, matcher, pNode) {}
    
    virtual node_ptr_direct* copy(node* new_pNode) override {
        return new node_ptr_direct(m_origin, matcher->copy(), new_pNode);
    }
};


//활성화 신호가 일정 이상 ~ 일정 이하를 받았을 때만 
//활성화 신호를 전달하는 노드포인터 (내부 카운터 존재)
class compiled::node_ptr_inner_counter : public node_ptr {
private:
    size_t lower_bound;
    size_t upper_bound;

    size_t counter;
    size_t prev_istart;


protected:
    virtual void transition_action(vector<active_request_info>& next_active, size_t state_istart) override {
        if (prev_istart != state_istart) {
            prev_istart = state_istart;
            counter = 0;
        }
        counter++;

        //범위에 들었을 경우만 state를 전달한다
        if (lower_bound <= counter && counter <= upper_bound)
            next_active.push_back({ m_target, state_istart });
    }

    virtual void clear_flag() override {
        counter = 0;
        prev_istart = node_ptr::INF;
    }


public:
    node_ptr_inner_counter(
        node* pOrigin, Imatchable* matcher, node* pNode, size_t low_bound = 0, size_t up_bound = node_ptr::INF) 
        : node_ptr(pOrigin, matcher, pNode), 
        lower_bound(low_bound), upper_bound(up_bound) 
    {
        clear_flag();
    }    
    virtual node_ptr_inner_counter* copy(node* new_pNode) override {
        return new node_ptr_inner_counter(
            m_origin, matcher->copy(), new_pNode, lower_bound, upper_bound);
    }
};


} //end of namespace assignment1
#endif