#ifndef __MY_SIMPLE_REGEX_H__
#define __MY_SIMPLE_REGEX_H__
#include <iostream>
#include <vector>

namespace assignment1 {
using namespace std;


// 특정한 구간을 표현하는 문자열
struct ranged_string {
    const string ref;          //참조 문자열
    const bool is_valid;        //객체 유효성
    const unsigned start, end;  //참조 문자열에서의 범위

    ranged_string(const string& ref, const unsigned start, const unsigned end, const bool is_valid) :
        ref(ref), start(start), end(end), is_valid(is_valid) {}

    //범위
    pair<unsigned, unsigned> span() const { return pair<unsigned, unsigned>(start, end); }

    //범위에 속하는 문자열 반환
    string group() const { return ref.substr(start, size_t(end - start)); }

    //유효하지 않은 ranged_string 객체를 반환한다.
    static const ranged_string invalid() { return ranged_string(string(), 0, 0, false); }
};


//Regex를 입력으로 받아 패턴을 인식하는 클래스
class MySimpleRegex {
private: MySimpleRegex();
    //이 클래스는 static 메서드만 있으므로 생성자를 private로 둔다.

public:
    class compiled;
    
    //정규표현식을 컴파일한 객체를 반환한다.
    static compiled compile(const string& m_regex, int test); 

    //source에서 정규표현식과 가장 먼저 일치하는 범위(유효하지 않을 수도 있음)를 구한다.
    static ranged_string match(const string& m_regex, const string& source);   

    //source에서 정규표현식과 일치하는 모든 범위를 구한다. 일치정보가 없을 경우 빈 vector가 반환된다.
    static vector<ranged_string> match_all(const string m_regex, const string& source);
};



//정규표현식을 컴파일한 객체로, 확인할 문자열을 입력하면 일치 정보를 반환한다.
class MySimpleRegex::compiled {
private:    
    /****************************/
    /*       inner classes      */
    /****************************/


    class Imatchable;   //매치 객체 인터페이스    
    class match_single; //단일 문자 매치    
    class match_dot;    //모든 문자 매치

    class node;             //유한 오토마타를 구성하는 노드
    class node_ptr;         //노드를 가리키는 포인터 (가상 클래스)
    class node_ptr_direct;  //노드를 직접 연결하는 포인터
                            //전이 요청을 받으면 무조건 전이 신호 보내줌
    class node_ptr_cnt_inner;   //노드를 조건부로 연결하는 포인터
                                //전이 요청을 일정 횟수 받은 경우에만 전이 신호 보내줌

    //해당 노드에 요청할 때 전달하는 정보
    struct active_request_info {
        node* target;
        unsigned start_index;
    };

    class node {
    private:
        string m_name;              // state node name
        unsigned m_state_istart;    // state가 첫 active되었을 때 입력되었던 문자의 인덱스
        bool m_state_active;        // 0: deactive, 1: active
        bool m_is_terminal;         // 0: terminal, 1: non-terminal

        vector<node_ptr*> next; // next link
        vector<node*> reverse_ref; //이 노드를 가리키는 역참조

    public:
        //생성자
        node(string name = "", bool isTerminal = false) :
            m_name(name), m_is_terminal(isTerminal), next(0) {
            m_state_istart = 0;
            m_state_active = false;
        }

        //소멸자: new로 할당받은 메모리를 반환한다.
        ~node() {
            for (int i = 0; i < next.size(); i++)
                if (next[i] != nullptr)
                    delete next[i];
        }

        unsigned index_start() const;
        void addNode(node_ptr* _next);
        void add_link_reverse_ref(node* ref);
        void active(int state_istart);
        void clear_flags();
        bool is_accepted() const;
        void input(vector<active_request_info>& next_active, const char ch);
    };

    class node_ptr {
    protected:
        node* m_pNode;
        Imatchable* matcher;

        //각 포인터가 수행할 전이 동작을 수행한다.
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) = 0;

    public:
        static const unsigned INF = ~0U; //2^32 - 1

        node_ptr(Imatchable* ranged_string, node* pNode) : m_pNode(pNode), matcher(ranged_string) {}
        ~node_ptr();

        //포인터와 관련된 플래그를 클리어한다. 필요한 경우만 오버라이드한다.
        virtual void clear_flag() {}

        //연결된 노드에 active를 요청한다. 한번에 될 수도 있고, 그렇지 않을 수도 있음
        virtual void active_request(
            vector<active_request_info>& next_active,
            unsigned state_istart, char ch
        ) final;

        //연결된 노드의 역참조를 주어진 노드로 연결한다
        virtual void link_reverse_ref(node* ref) final {
            m_pNode->add_link_reverse_ref(ref);
        }
    };

    class node_ptr_direct : public node_ptr {
    protected:
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) override {
            next_active.push_back({ m_pNode, state_istart });
        }

    public:
        node_ptr_direct(Imatchable* matcher, node* pNode) : node_ptr(matcher, pNode) {}

    };

    class node_ptr_cnt_inner : public node_ptr {
    private:
        unsigned lower_bound;
        unsigned upper_bound;

        unsigned counter;
        unsigned prev_istart;

    protected:
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) override {
            if (prev_istart != state_istart) {
                prev_istart = state_istart;
                counter = 0;
            }
            counter++;

            //범위에 들었을 경우만 state를 전달한다
            if (lower_bound <= counter && counter <= upper_bound)
                next_active.push_back({ m_pNode, state_istart });
        }

    public:
        node_ptr_cnt_inner(Imatchable* matcher, node* pNode, unsigned low_bound = 0, unsigned up_bound = node_ptr::INF) :
            node_ptr(matcher, pNode), lower_bound(low_bound), upper_bound(up_bound) {
            clear_flag();
        }

        virtual void clear_flag() override {
            counter = 0;
            prev_istart = node_ptr::INF;
        }
    };

    /****************************/
    /*      Private fields      */
    /****************************/
    vector<node*> m_node;           //상태기계 노드 컨테이너
    vector<node*> m_get_epsilon;    //엡실론 신호를 받는 노드리스트
    vector<node*> m_terminal;       //터미널 노드리스트
    const string& m_regex;          //저장된 정규표현식

    /****************************/
    /*    Private functions     */
    /****************************/
    void create_state_machine(int test);    //상태기계 노드 컨테이너에 상태기계를 생성한다.    
    void delete_state_machine();            //생성된 상태기계를 삭제한다.   
    ranged_string state_machine_input(    //상태기계에 문자열을 입력으로 받고, 
                                    //가장 처음으로 accept된 일치 정보를 출력한다.
        const string& src, 
        unsigned index_start = 0,
        bool check_at_front_only = false);


public:
    //주어진 정규표현식으로 내부적으로 상태기계를 생성한다.
    compiled(const string& m_regex = "", int test = 0) : m_regex(m_regex) {
        create_state_machine(test);
    }

    //소멸자 (메모리 반환 필요)
    ~compiled() { delete_state_machine(); }

    //source에서 start_idx부터 탐색을 시작해 가장 처음으로 발견된 일치 정보를 반환한다.
    ranged_string match(const string& source, unsigned index_start = 0) {
        return state_machine_input(source, index_start, false);
    }

    //source에서 발견되는 모든 일치 정보를 반환한다.
    vector<ranged_string> match_all(const string& source) {
        vector<ranged_string> ret;
        unsigned found_idx = 0;
        while (found_idx < source.size()) {
            ranged_string ans = match(source, found_idx);
            if (!ans.is_valid) break;
            ret.push_back(ans);
            found_idx = ret.back().end;
        }
        return ret;
    }
};


} //end of namespace assignment1
#endif