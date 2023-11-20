#ifndef __MY_SIMPLE_REGEX_NODE_H__
#define __MY_SIMPLE_REGEX_NODE_H__
#include "MySimpleRegex.h"


/*********************************/
/*    class compiled's           */
/*    Inner Classes & members    */
/*********************************/
namespace assignment1 {


//���� �ӽ��� ��带 ǥ���ϴ� Ŭ����
class compiled::node {
    friend class node_ptr;
private:
    string m_name;              // state node name
    size_t m_state_istart;    // state�� ù active�Ǿ��� �� �ԷµǾ��� ������ �ε���
    bool m_state_active;        // 0: deactive, 1: active
    bool m_is_terminal;         // 0: terminal, 1: non-terminal

    vector<node_ptr*> m_next;     // next link
    vector<const node*> m_reverse_ref;  //�� ��带 ����Ű�� ������

    void add_link_reverse_ref(node* ref); //����ũ�� �߰��Ѵ�.

public:
    node(string name, bool isTerminal = false);
    ~node();

    /*********  ��ȸ �Լ�  *********/
    const string& name() const;
    size_t index_start() const;   //is_accepted�� �� �νĵ� ���ڿ��� ù ��ġ�� �ǹ��Ѵ�.  
    bool is_accepted() const;       //�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
    const vector<node_ptr*>& next() const;      //next ����� ��ȯ�Ѵ�.
    const vector<const node*>& reverse_ref() const;   //������ ��������� ��ȯ�Ѵ�.

    /*********  ���� �Լ�  *********/
    void add_link(node_ptr* _next); //�� ��尡 ����Ű�� ��带 �߰��Ѵ�.
    void active(size_t state_istart); //��带 Ȱ��ȭ ���·� �����.
    void clear_flags(); //��� ���¿� ���õ� ���� �÷��׵��� ��� Ŭ�����Ѵ�.
    void set_terminal(bool is_terminal);


    //��忡 �� ���ڸ� �Է��Ѵ�. 
    //��尡 Ȱ��ȭ���� �� ���ڰ� ��ġ�Ǹ� next ���鿡�� Ȱ��ȭ�� ��û�Ѵ�.
    //��� Ȱ��ȭ && �͹̳� ����� ��� is_accepted�� true�� �ȴ�.
    void input(vector<active_request_info>& next_active, const char ch);
};


//node�� ����Ű�� Ư���� ������ �߻� Ŭ����
class compiled::node_ptr {
    friend class node;
protected:
    node* m_pNode;
    Imatchable* matcher;

    //�� �����Ͱ� ������ ���� ������ �����Ѵ�. �������̵� �ʿ�
    virtual void transition_action(
        vector<active_request_info>& next_active, size_t state_istart) = 0;

    //����� ����� �������� �־��� ���� �����Ѵ�
    virtual void link_reverse_ref(node* ref) final {
        m_pNode->add_link_reverse_ref(ref);
    }

    //�����Ϳ� ���õ� �÷��׸� Ŭ�����Ѵ�. �ʿ��� ��츸 �������̵��Ѵ�.
    virtual void clear_flag() {}


public:
    static const size_t INF = ~0ULL; //2^32 - 1

    node_ptr(Imatchable* ranged_string, node* pNode) 
        : m_pNode(pNode), matcher(ranged_string) {}
    ~node_ptr() {
        if (matcher != nullptr)
            delete matcher; //���� �Ҵ�� matcher�� ��ȯ�Ѵ�.
    }

    //node*���� ����� ����ȯ
    explicit operator node*() { return m_pNode; }

    //���� �����͸� ���ο� ������ ���� �����ͷ� �����Ѵ�.
    virtual node_ptr* copy(node* new_pNode) = 0;

    //����� ��尡 ���� ����Ŭ�� active �ǵ��� ��û�Ѵ�.
    //�ѹ��� �� ���� �ְ�, �׷��� ���� ���� ����
    virtual void request_active_nexttime(
        vector<active_request_info>& next_active, 
        size_t state_istart, char ch ) final 
    { 
        if (matcher->test(ch)) //�䱸�Ǵ� ���ڿ� ��ġ�Ǵ� ��쿡�� ���� �ൿ�� ���Ѵ�.
            transition_action(next_active, state_istart);
    }

    //is_accepted�� �� �νĵ� ���ڿ��� ù ��ġ�� �ǹ��Ѵ�.
    size_t index_start() const {
        return m_pNode->index_start();
    }

    //�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
    bool is_accepted() const {
        return m_pNode->is_accepted();
    }

    //��� �����Ͱ� ����Ű�� ���� �Ķ���� ��尡 ������ Ȯ���Ѵ�.
    bool is_ref_equal(const node* pNode) const {
        return m_pNode == pNode;
    }
};


//Ȱ��ȭ ��ȣ�� �ٷ� �����ϴ� ��� ������
class compiled::node_ptr_direct : public node_ptr {
protected:
    virtual void transition_action(vector<active_request_info>& next_active, size_t state_istart) override {
        next_active.push_back({ m_pNode, state_istart });
    }

public:
    node_ptr_direct(Imatchable* matcher, node* pNode) : node_ptr(matcher, pNode) {}
    
    virtual node_ptr_direct* copy(node* new_pNode) override {
        return new node_ptr_direct(matcher->copy(), new_pNode);
    }
};


//Ȱ��ȭ ��ȣ�� ���� �̻� ~ ���� ���ϸ� �޾��� ���� 
//Ȱ��ȭ ��ȣ�� �����ϴ� ��������� (���� ī���� ����)
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

        //������ ����� ��츸 state�� �����Ѵ�
        if (lower_bound <= counter && counter <= upper_bound)
            next_active.push_back({ m_pNode, state_istart });
    }

    virtual void clear_flag() override {
        counter = 0;
        prev_istart = node_ptr::INF;
    }


public:
    node_ptr_inner_counter(Imatchable* matcher, node* pNode, size_t low_bound = 0, size_t up_bound = node_ptr::INF) :
        node_ptr(matcher, pNode), lower_bound(low_bound), upper_bound(up_bound) {
        clear_flag();
    }    
    virtual node_ptr_inner_counter* copy(node* new_pNode) override {
        return new node_ptr_inner_counter(
            matcher->copy(), new_pNode, lower_bound, upper_bound);
    }
};


} //end of namespace assignment1
#endif