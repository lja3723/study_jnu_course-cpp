#ifndef __MY_SIMPLE_REGEX_H__
#define __MY_SIMPLE_REGEX_H__
#include <iostream>
#include <vector>

namespace assignment1 {
using namespace std;


// Ư���� ������ ǥ���ϴ� ���ڿ�
struct ranged_string {
    const string ref;          //���� ���ڿ�
    const bool is_valid;        //��ü ��ȿ��
    const unsigned start, end;  //���� ���ڿ������� ����

    ranged_string(const string& ref, const unsigned start, const unsigned end, const bool is_valid) :
        ref(ref), start(start), end(end), is_valid(is_valid) {}

    //����
    pair<unsigned, unsigned> span() const { return pair<unsigned, unsigned>(start, end); }

    //������ ���ϴ� ���ڿ� ��ȯ
    string group() const { return ref.substr(start, size_t(end - start)); }

    //��ȿ���� ���� ranged_string ��ü�� ��ȯ�Ѵ�.
    static const ranged_string invalid() { return ranged_string(string(), 0, 0, false); }
};


//Regex�� �Է����� �޾� ������ �ν��ϴ� Ŭ����
class MySimpleRegex {
private: MySimpleRegex();
    //�� Ŭ������ static �޼��常 �����Ƿ� �����ڸ� private�� �д�.

public:
    class compiled;
    
    //����ǥ������ �������� ��ü�� ��ȯ�Ѵ�.
    static compiled compile(const string& m_regex, int test); 

    //source���� ����ǥ���İ� ���� ���� ��ġ�ϴ� ����(��ȿ���� ���� ���� ����)�� ���Ѵ�.
    static ranged_string match(const string& m_regex, const string& source);   

    //source���� ����ǥ���İ� ��ġ�ϴ� ��� ������ ���Ѵ�. ��ġ������ ���� ��� �� vector�� ��ȯ�ȴ�.
    static vector<ranged_string> match_all(const string m_regex, const string& source);
};



//����ǥ������ �������� ��ü��, Ȯ���� ���ڿ��� �Է��ϸ� ��ġ ������ ��ȯ�Ѵ�.
class MySimpleRegex::compiled {
private:    
    /****************************/
    /*       inner classes      */
    /****************************/


    class Imatchable;   //��ġ ��ü �������̽�    
    class match_single; //���� ���� ��ġ    
    class match_dot;    //��� ���� ��ġ

    class node;             //���� ���丶Ÿ�� �����ϴ� ���
    class node_ptr;         //��带 ����Ű�� ������ (���� Ŭ����)
    class node_ptr_direct;  //��带 ���� �����ϴ� ������
                            //���� ��û�� ������ ������ ���� ��ȣ ������
    class node_ptr_cnt_inner;   //��带 ���Ǻη� �����ϴ� ������
                                //���� ��û�� ���� Ƚ�� ���� ��쿡�� ���� ��ȣ ������

    //�ش� ��忡 ��û�� �� �����ϴ� ����
    struct active_request_info {
        node* target;
        unsigned start_index;
    };

    class node {
    private:
        string m_name;              // state node name
        unsigned m_state_istart;    // state�� ù active�Ǿ��� �� �ԷµǾ��� ������ �ε���
        bool m_state_active;        // 0: deactive, 1: active
        bool m_is_terminal;         // 0: terminal, 1: non-terminal

        vector<node_ptr*> next; // next link
        vector<node*> reverse_ref; //�� ��带 ����Ű�� ������

    public:
        //������
        node(string name = "", bool isTerminal = false) :
            m_name(name), m_is_terminal(isTerminal), next(0) {
            m_state_istart = 0;
            m_state_active = false;
        }

        //�Ҹ���: new�� �Ҵ���� �޸𸮸� ��ȯ�Ѵ�.
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

        //�� �����Ͱ� ������ ���� ������ �����Ѵ�.
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) = 0;

    public:
        static const unsigned INF = ~0U; //2^32 - 1

        node_ptr(Imatchable* ranged_string, node* pNode) : m_pNode(pNode), matcher(ranged_string) {}
        ~node_ptr();

        //�����Ϳ� ���õ� �÷��׸� Ŭ�����Ѵ�. �ʿ��� ��츸 �������̵��Ѵ�.
        virtual void clear_flag() {}

        //����� ��忡 active�� ��û�Ѵ�. �ѹ��� �� ���� �ְ�, �׷��� ���� ���� ����
        virtual void active_request(
            vector<active_request_info>& next_active,
            unsigned state_istart, char ch
        ) final;

        //����� ����� �������� �־��� ���� �����Ѵ�
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

            //������ ����� ��츸 state�� �����Ѵ�
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
    vector<node*> m_node;           //���±�� ��� �����̳�
    vector<node*> m_get_epsilon;    //���Ƿ� ��ȣ�� �޴� ��帮��Ʈ
    vector<node*> m_terminal;       //�͹̳� ��帮��Ʈ
    const string& m_regex;          //����� ����ǥ����

    /****************************/
    /*    Private functions     */
    /****************************/
    void create_state_machine(int test);    //���±�� ��� �����̳ʿ� ���±�踦 �����Ѵ�.    
    void delete_state_machine();            //������ ���±�踦 �����Ѵ�.   
    ranged_string state_machine_input(    //���±�迡 ���ڿ��� �Է����� �ް�, 
                                    //���� ó������ accept�� ��ġ ������ ����Ѵ�.
        const string& src, 
        unsigned index_start = 0,
        bool check_at_front_only = false);


public:
    //�־��� ����ǥ�������� ���������� ���±�踦 �����Ѵ�.
    compiled(const string& m_regex = "", int test = 0) : m_regex(m_regex) {
        create_state_machine(test);
    }

    //�Ҹ��� (�޸� ��ȯ �ʿ�)
    ~compiled() { delete_state_machine(); }

    //source���� start_idx���� Ž���� ������ ���� ó������ �߰ߵ� ��ġ ������ ��ȯ�Ѵ�.
    ranged_string match(const string& source, unsigned index_start = 0) {
        return state_machine_input(source, index_start, false);
    }

    //source���� �߰ߵǴ� ��� ��ġ ������ ��ȯ�Ѵ�.
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