//�� ������ ���� https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;


namespace assignment1 {

class MySimpleRegex {    
private: MySimpleRegex();   //�� Ŭ������ static �޼��常 �����Ƿ� �����ڸ� private�� �д�.
public:
    class matched;  //���ڿ����� ���ϰ� ��ġ�� ������ ǥ���Ѵ�.
    class compiled; //����ǥ������ �������� ��ü��, Ȯ���� ���ڿ��� �Է��ϸ� ��ġ ������ ��ȯ�Ѵ�.
    
    //����ǥ������ �������� ��ü�� ��ȯ�Ѵ�.
    static compiled compile(const string& m_regex, int test_case);    
    //source���� ����ǥ���İ� ���� ���� ��ġ�ϴ� ����(��ȿ���� ���� ���� ����)�� ���Ѵ�.
    static matched match(const string& m_regex, const string& source);    
    //source���� ����ǥ���İ� ��ġ�ϴ� ��� ������ ���Ѵ�. ��ġ������ ���� ��� �� vector�� ��ȯ�ȴ�.
    static vector<matched> match_all(const string m_regex, const string& source);
};

// ���ڿ����� ���ϰ� ��ġ�� ������ ǥ���Ѵ�.
class MySimpleRegex::matched {
private:
    const string& m_ref;            //���� ���ڿ�
    const bool m_valid;             //��ü ��ȿ��
    const unsigned m_start, m_end;  //���� ���ڿ������� ����

public:
    matched(const string& ref, unsigned start, unsigned end, bool valid) :
        m_ref(ref), m_start(start), m_end(end), m_valid(valid) {}

    //�ش� ��ü�� ��ȿ�� �� ���� Ȯ�� (false�� ��ȿ���� ����)
    bool valid() const { return m_valid; }

    //������ ���ϴ� ���ڿ� ��ȯ
    string group() const { return m_ref.substr(m_start, m_end - m_start); }

    //������ ���۰�
    unsigned start() const { return m_start; }

    //������ ����
    unsigned end() const { return m_end; }

    //����
    pair<unsigned, unsigned> span() const { return pair<unsigned, unsigned>(m_start, m_end); }

    //��ȿ���� ���� matched ��ü�� ��ȯ�Ѵ�.
    static const matched invalid() { return matched("", 0, 0, false); }
};

//����ǥ������ �������� ��ü��, Ȯ���� ���ڿ��� �Է��ϸ� ��ġ ������ ��ȯ�Ѵ�.
class MySimpleRegex::compiled {
private:
    /****************************/
    /*       Inner Classes      */
    /****************************/

    /*��ġ ��ü �������̽�*/ class Imatchable {
    public:
        virtual bool test(char ch) = 0;
    };
    /*���� ���� ��ġ*/ class match_single : public Imatchable {
    private: char ch;
    public:
        match_single(char ch) : ch(ch) {}
        virtual bool test(char _ch) override { return ch == _ch; }
    };
    /*��� ���� ��ġ*/ class match_dot : public Imatchable {
    public:
        virtual bool test(char _ch) override {
            return ('A' <= _ch && _ch <= 'Z') || ('a' <= _ch && _ch <= 'z');
        }
    };

    /****************************/
    
    class node; //pre-defined class

    //��Ƽ�긦 ��û�� �� �����ϴ� ����
    struct active_request_info {
        node* target;
        unsigned start_index;
    };

    //��带 ����Ű�� ������(���� Ŭ����)
    class node_ptr {
    protected: 
        node* m_pNode;
        Imatchable* matcher;

        //�� �����Ͱ� ������ ���� ������ �����Ѵ�.
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) = 0;

    public:
        static const unsigned INF = ~0U; //2^32 - 1
        
        node_ptr(Imatchable* match, node* pNode) : m_pNode(pNode), matcher(match) {}
        ~node_ptr() { if (matcher != nullptr) delete matcher; }

        //�����Ϳ� ���õ� �÷��׸� Ŭ�����Ѵ�. �ʿ��� ��츸 �������̵��Ѵ�.
        virtual void clear_flag() {}
        
        //����� ��忡 active�� ��û�Ѵ�. �ѹ��� �� ���� �ְ�, �׷��� ���� ���� ����
        virtual void active_request(vector<active_request_info>& next_active, unsigned state_istart, char ch) final {
            //�䱸�Ǵ� ���ڿ� ��ġ�Ǵ� ��쿡�� �׼��� ����
            if (matcher->test(ch)) transition_action(next_active, state_istart);
        }
        
        //����� ����� �������� �־��� ���� �����Ѵ�
        virtual void link_reverse_ref(node* ref) final {
            m_pNode->add_link_reverse_ref(ref);
        }      
    };

    //��带 ���� �����ϴ� ������
    //���� ��û�� ������ ������ ���� ��ȣ ������
    class node_ptr_direct : public node_ptr {
    protected:
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) override {
            next_active.push_back({ m_pNode, state_istart });
        }

    public:
        node_ptr_direct(Imatchable* matcher, node* pNode) : node_ptr(matcher, pNode) {}

    };

    //��带 ���Ǻη� �����ϴ� ������
    //���� ��û�� ���� Ƚ�� ���� ��쿡�� ���� ��ȣ ������
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

    //���� ���丶Ÿ�� �����ϴ� ���
    class node {
    private:
        string m_name; // state node name
        unsigned m_state_istart;    // state�� ù active�Ǿ��� �� �ԷµǾ��� ������ �ε���
        bool m_state_active;        // 0: deactive, 1: active

        bool m_is_terminal;  // 0: terminal, 1: non-terminal

        vector<node_ptr*> next; // next link
        vector<node*> reverse_ref; //�� ��带 ����Ű�� ������

    public:
        //�ݺ� �������� ��� �����ϴ�. ���Ѵ븦 �ǹ��Ѵ�.
        //static const unsigned INF = unsigned(-1); //2^31 - 1

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

        //ó������ Ȱ��ȭ�� ���¿��� �� ù �ε����� ��ȯ�Ѵ�.
        //is_accepted�� �� �νĵ� ���ڿ��� ù ��ġ�� �ǹ��Ѵ�.
        unsigned index_start() const { return m_state_istart; }

        //�� ��尡 ����Ű�� ��带 �߰��Ѵ�.
        void addNode(node_ptr* _next) { 
            next.push_back(_next); 
            _next->link_reverse_ref(this);
        }
    
        //����ũ�� �߰��Ѵ�.
        void add_link_reverse_ref(node* ref) {
            reverse_ref.push_back(ref);
        }

        //��带 Ȱ��ȭ ���·� �����.
        void active(int state_istart) {
            m_state_istart = state_istart;
            m_state_active = true;
        }

        //��� ���¿� ���õ� ���� �÷��׵��� ��� Ŭ�����Ѵ�.
        void clear_flags() {
            m_state_istart = 0;
            m_state_active = false;

            for (node_ptr* p : next)
                p->clear_flag();
        }

        //�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
        bool is_accepted() const {
            return m_is_terminal && m_state_active;
        }

        //��忡 �� ���ڸ� �Է���: ��尡 Ȱ��ȭ���� �� ���ڰ� ��Ī�Ǹ� ���� ���鿡�� Ȱ��ȭ�� ��û��
        //��� Ȱ��ȭ & ���� ��Ī�� �� �͹̳� ����� ��� is_accepted�� true�� ��
        void input(vector<active_request_info>& next_active, const char ch) {
            if (!m_state_active) return;

            for (int i = 0; i < next.size(); i++) {
                next[i]->active_request(next_active, m_state_istart, ch);
            }

            //�νĵǾ��� ��� true
            //m_isAccepted = m_state_active && match && m_isTerminal;
            m_state_active = false; // deactivate this node after matching
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

    //TODO: �־��� ����ǥ�������� ���¸ӽ� �����ϱ� �ʿ�
    //���±�� ��� �����̳ʿ� ���±�踦 �����Ѵ�.
    void create_state_machine(int test_case) {

        //mock implementation
        //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
        if (test_case == 1) {
            // abc|ade
            m_node.resize(6/*����ǥ���� �ؼ� �� ��� ������ �Ǿ�� ��*/);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);
            m_node[4] = new node("s4");
            m_node[5] = new node("s5");

            // set links
            m_node[0]->addNode(new node_ptr_direct(new match_single('a'), m_node[1]));
            m_node[0]->addNode(new node_ptr_direct(new match_single('a'), m_node[4]));

            m_node[1]->addNode(new node_ptr_direct(new match_single('b'), m_node[2]));
            m_node[2]->addNode(new node_ptr_direct(new match_single('c'), m_node[3]));

            m_node[4]->addNode(new node_ptr_direct(new match_single('d'), m_node[5]));
            m_node[5]->addNode(new node_ptr_direct(new match_single('e'), m_node[3]));

            // ���Ƿ� ��ȣ �޴� ��� ����
            m_get_epsilon.push_back(m_node[0]);

            // �͹̳� ��� ����
            m_terminal.push_back(m_node[3]);
        }
        else if (test_case == 2) {
            // aba
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('a'), m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('b'), m_node[2]));
            m_node[2]->addNode(new node_ptr_direct(new match_single('a'), m_node[3]));

            m_get_epsilon.push_back(m_node[0]);

            m_terminal.push_back(m_node[3]);
        }
        else if (test_case == 3) {
            // NK((abc|ABC)*N|(OP)+)Q
            m_node.resize(12);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3");
            m_node[4] = new node("s4");
            m_node[5] = new node("s5");
            m_node[6] = new node("s6");
            m_node[7] = new node("s7");
            m_node[8] = new node("s8");
            m_node[9] = new node("s9");
            m_node[10] = new node("s10");
            m_node[11] = new node("s11", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('N'), m_node[1]));

            m_node[1]->addNode(new node_ptr_direct(new match_single('K'), m_node[5]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('K'), m_node[2]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('K'), m_node[8]));

            m_node[2]->addNode(new node_ptr_direct(new match_single('a'), m_node[3]));
            m_node[2]->addNode(new node_ptr_direct(new match_single('A'), m_node[6]));

            m_node[3]->addNode(new node_ptr_direct(new match_single('b'), m_node[4]));

            m_node[4]->addNode(new node_ptr_direct(new match_single('c'), m_node[2]));
            m_node[4]->addNode(new node_ptr_direct(new match_single('c'), m_node[5]));

            m_node[5]->addNode(new node_ptr_direct(new match_single('N'), m_node[10]));

            m_node[6]->addNode(new node_ptr_direct(new match_single('B'), m_node[7]));

            m_node[7]->addNode(new node_ptr_direct(new match_single('C'), m_node[2]));
            m_node[7]->addNode(new node_ptr_direct(new match_single('C'), m_node[5]));

            m_node[8]->addNode(new node_ptr_direct(new match_single('O'), m_node[9]));

            m_node[9]->addNode(new node_ptr_direct(new match_single('P'), m_node[8]));
            m_node[9]->addNode(new node_ptr_direct(new match_single('P'), m_node[10]));

            m_node[10]->addNode(new node_ptr_direct(new match_single('Q'), m_node[11]));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[11]);
        }
        else if (test_case == 4) {
            // ab+c
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('a'), m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('b'), m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('b'), m_node[2]));
            m_node[2]->addNode(new node_ptr_direct(new match_single('c'), m_node[3]));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[3]);
        }
        else if (test_case == 5) {
            // abc+
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('a'), m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('b'), m_node[2]));
            m_node[2]->addNode(new node_ptr_direct(new match_single('c'), m_node[2]));
            m_node[2]->addNode(new node_ptr_direct(new match_single('c'), m_node[3]));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[3]);
        }
        else if (test_case == 6) {
            // aB{3,6}c
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('a'), m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('B'), m_node[1]));
            //���ο� ���������!!
            m_node[1]->addNode(new node_ptr_cnt_inner(new match_single('B'), m_node[2], 3, 6));
            m_node[2]->addNode(new node_ptr_direct(new match_single('c'), m_node[3]));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[3]);
        }
        else if (test_case == 7) {
            // jkT{4,5}
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('j'), m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(new match_single('k'), m_node[2]));
            m_node[2]->addNode(new node_ptr_direct(new match_single('T'), m_node[2]));
            m_node[2]->addNode(new node_ptr_cnt_inner(new match_single('T'), m_node[3], 4, 5));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[3]);
            }
        else if (test_case == 8) {
            //T{3,5}
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('T'), m_node[1]));
            m_node[0]->addNode(new node_ptr_cnt_inner(new match_single('T'), m_node[1], 3, 5));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[1]);
        }
        else if (test_case == 9) {
            //T+
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('T'), m_node[1]));
            m_node[0]->addNode(new node_ptr_direct(new match_single('T'), m_node[1]));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[1]);
        }
}

    //������ ���±�踦 �����Ѵ�.
    void delete_state_machine() {
        for (int i = 0; i < m_node.size(); i++)
            if (m_node[i] != nullptr)
                delete m_node[i];
    }

    //TODO: T* , T+ ���� ���̽����� �׸����ϰ� �����ϰ� �ٲ�� ��
    //���±�迡 ���ڿ��� �Է����� �ް�, ���� ó������ accept�� ��ġ ������ ����Ѵ�.
    matched state_machine_input(const string& src, unsigned index_start = 0, bool check_at_front_only = false) {
        map<int, matched*> found;
        vector<node*> actives;
        vector<active_request_info> next_actives;

        ///*debug*/cout << "test for \"" << source << "\"" << endl;
        for (unsigned i = index_start; i < src.length(); i++) {
            ///*debug*/cout << "  >> input " << src[i] << endl;
            //���Ƿ� ��ȣ�� �ο�
            //check_at_front_only�� ���ڿ� ������ ���ϰ� ��ġ�ϴ� ��츸 Ȯ���Ѵ�.
            //�� ���� 1ȸ�� ���Ƿ� ��ȣ�� �ο��Ѵ�.
            if (!check_at_front_only || i == 0) {
                for (node*& target : m_get_epsilon) {
                    target->active(i);
                    actives.push_back(target);
                }
            }
                
            // give ch all nodes
            // nv[j]�� active ���¿��ٸ� �� ���� �� nv[j]���� ������ ȭ��ǥ��
            // ������ ������ �� ����Ű�� ����� transited�� Ȱ��ȭ�Ѵ�.
            while (!actives.empty()) {
                node* target = actives.back();
                target->input(next_actives, src[i]);
                actives.pop_back();
            }

            //���� Ȱ��ȭ�� ��� ����Ʈ�� �޾ƿ��� Ȱ��ȭ�Ѵ�.
            while (!next_actives.empty()) {
                active_request_info& info = next_actives.back();
                (info.target)->active(info.start_index);
                actives.push_back(info.target);
                next_actives.pop_back();
            }

            //�͹̳� ��� ��ȸ �� accepted�Ǿ����� matched�� �����Ѵ�.
            for (node* terminal : m_terminal) {
                if (terminal->is_accepted()) {
                    ///*debug*/cout << "       active_count: " << nv[j].active_count() << endl;
                    unsigned istart = terminal->index_start();
                    if (found.find(istart) != found.end()) delete found[istart];
                    found[istart] = new matched(src, istart, i + 1, true);
                }
            }
        }
        
        ///*debug*/cout << endl;
        //src�� ��� input�� �� ��� ����� �÷��� �ʱ�ȭ
        for (node* target : m_node) target->clear_flags();


        if (found.empty()) //��ġ�ϴ� ���� ����
            return matched::invalid();
        else {
            matched ret = *found.begin()->second;
            for (auto& p : found)
                delete p.second;

            return ret;
        }
    }



public:
    //�־��� ����ǥ�������� ���������� ���±�踦 �����Ѵ�.
    compiled(const string& m_regex = "", int test_case = 0) : m_regex(m_regex) {
        create_state_machine(test_case);
    }

    //�Ҹ��� (�޸� ��ȯ �ʿ�)
    ~compiled() { delete_state_machine(); }

    //source���� start_idx���� Ž���� ������ ���� ó������ �߰ߵ� ��ġ ������ ��ȯ�Ѵ�.
    matched match(const string& source, unsigned index_start = 0) {
        return state_machine_input(source, index_start, false);
    }

    //source���� �߰ߵǴ� ��� ��ġ ������ ��ȯ�Ѵ�.
    vector<matched> match_all(const string& source) {
        vector<matched> ret;
        unsigned found_idx = 0;
        while (found_idx < source.size()) {
            matched ans = match(source, found_idx);
            if (!ans.valid()) break;
            ret.push_back(ans);
            found_idx = ret.back().end();
        }
        return ret;
    }
};



MySimpleRegex::compiled MySimpleRegex::compile(const string& m_regex, int test_case = 0) {
    return compiled(m_regex, test_case);
}
MySimpleRegex::matched MySimpleRegex::match(const string& m_regex, const string& source) {
    return compiled(m_regex).match(source);
}
vector<MySimpleRegex::matched> MySimpleRegex::match_all(const string m_regex, const string& source) {
    return compiled(m_regex).match_all(source);
}



class unit_tester {
    using match = MySimpleRegex::matched;
    using matches = vector<match>;
private:
    bool view_details;
    vector<int> test_numbers;
    vector<string> regexes;
    vector<vector<string>> tests;
    vector<vector<matches>> expects;

    void run_tests() {
        if (!test_disables.empty()) {
            cout << "[��ŵ�� �׽�Ʈ ���]  ";
            for (int skip : test_disables) cout << skip << ";  ";
            cout << "\n\n\n";
        }

        for (int test = 1; test < test_numbers.size(); test++) {
            if (test_disables.find(test) != test_disables.end()) continue; //�׽�Ʈ ��ŵ

            bool test_successed = true;
            bool regex_printed = false;
            string indent = "   ";
            vector<bool> results = run_test(test_numbers[test]); //test ����
            for (int i = 0; i < results.size(); i++) {
                if (!results[i]) {
                    test_successed = false;
                    //������ ���� ��� ����
                    MySimpleRegex::compiled cp = MySimpleRegex::compile(regexes[test], test);
                    matches result = cp.match_all(tests[test][i]);

                    //������ �׽�Ʈ ���� ���
                    if (!regex_printed) {
                        regex_printed = true;
                        cout << "\n\n\n";
                        cout << "<<<< �׽�Ʈ #" << test << " ���� >>>>" << endl;
                        cout << "����ǥ����: \"" << regexes[test] << "\"\n";
                    }

                    //������ ���̽� ���
                    cout << "���̽� #" << i << endl;
                    cout << indent << "< ��� ��� >" << endl;
                    cout << indent << "\"" << tests[test][i] << "\"\n";
                    unit_tester::match_info_print(expects[test][i], indent + " ");
                    for (const MySimpleRegex::matched& mt : expects[test][i])
                        cout << "\t<���� [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";

                    cout << indent << "< ���� ��� >" << endl;
                    cout << indent << "\"" << tests[test][i] << "\"\n";
                    unit_tester::match_info_print(result, indent + " ");
                    for (const MySimpleRegex::matched& mt : result)
                        cout << "\t<���� [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";


                }
                if (!test_successed && i + 1 == results.size()) cout << "\n\n";
            }

            if (test_successed) {
                if (view_details) {
                    cout << "\n\n\n";
                    cout << "<�׽�Ʈ #" << test << " ��� (detailed mode enabled)>" << endl;
                    cout << "����ǥ����: \"" << regexes[test] << "\"\n";

                    MySimpleRegex::compiled cp = MySimpleRegex::compile(regexes[test], test);
                    for (int i = 0; i < tests[test].size(); i++) {
                        vector<MySimpleRegex::matched> result = cp.match_all(tests[test][i]);
                        cout << indent << "\"" << tests[test][i] << "\"";
                        if (result.empty()) {
                            cout << " -> ��ġ���� ����" << endl;
                            continue;
                        }
                        cout << endl;

                        cout << indent << "< ���� ��� >" << endl;
                        cout << indent << "\"" << tests[test][i] << "\"\n";
                        unit_tester::match_info_print(result, indent + " ");
                        for (const MySimpleRegex::matched& mt : result)
                            cout << "\t<���� [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";
                    }
                    cout << "\n\n";
                }
                else cout << "�׽�Ʈ #" << test << " ���>" << endl;
            }
        }
    }

    bool assertEqual(matches& expect, matches& result) {
        for (int i = 0; i < expect.size(); i++) {
            if (expect[i].start() != result[i].start()) return false;
            if (expect[i].end() != result[i].end())     return false;
            if (expect[i].valid() != result[i].valid()) return false;
        }
        return true;
    }

    vector<bool> run_test(int test_case) {
        vector<bool> ret;

        MySimpleRegex::compiled cp = MySimpleRegex::compile(regexes[test_case], test_case);
        for (int i = 0; i < tests[test_case].size(); i++) {
            matches list_all = cp.match_all(tests[test_case][i]);
            ret.push_back(assertEqual(expects[test_case][i], list_all));
        }

        return ret;
    }

public:
    unit_tester() : view_details(false) { register_tests(); }

    void run() { run_tests(); }

    void show_details(bool mode) { view_details = mode; }

    //��ġ�� ������ ���� ǥ��
    void match_info_print(const vector<MySimpleRegex::matched>& result, const string& indent) {  
        cout << indent;
        int last_print = 0;
        for (const MySimpleRegex::matched& mt : result) {
            int len = mt.start() - last_print;
            for (int i = 0; i < len; i++) cout << " ";
            len = mt.end() - mt.start();
            for (int i = 0; i < len; i++) cout << "*";
            last_print = mt.end();
        }
        cout << endl;
    }

protected:
    //�������� ���� �׽�Ʈ����Ʈ
    set<int> test_disables = { };

    //������ �׽�Ʈ ���
    void register_tests() {
        test0(); //register example (<!> do not delete <!>)
        test1(); test2(); test3(); test4(); test5();
        test6(); test7(); test8(); test9();
    }

    //test func example
    void test0() {
        int t = 0;
        test_numbers.push_back(t);
        regexes.push_back("test_regex");
        tests.push_back({
            "test_str",
            "test_str"
            });
        expects.push_back({
            {
                match(tests[t][0], 0, 0, true)},
            {
                match(tests[t][1], 0, 0, true)}
            });
    }
    
    void test1() {
        int t = 1;
        test_numbers.push_back(t);
        regexes.push_back("abc|ade");
        tests.push_back( {
            "abc",
            "ade",
            "aaaaabbbbabcaaaaaadeaaaaabc",
            "abbbbbbbbdddddddfdfdfdfdf",
            "zzosiduabcfdsfdafdadere",
            "aosisudababaifudiaddadeidufodi",
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" 
        } );
        expects.push_back( {
            {
                match(tests[t][0], 0, 3, true)},
            {
                match(tests[t][1], 0, 3, true) },
            {
                match(tests[t][2], 9, 12, true),
                match(tests[t][2], 17, 20, true),
                match(tests[t][2], 24, 27, true) },
            { },
            {
                match(tests[t][4], 7, 10, true),
                match(tests[t][4], 18, 21, true), },
            {
                match(tests[t][5], 20, 23, true) },
            {
                match(tests[t][6], 23, 26, true),
                match(tests[t][6], 27, 30, true),
                match(tests[t][6], 100, 103, true),
                match(tests[t][6], 107, 110, true) }
        });      
    }
    void test2() {
        int t = 2;
        test_numbers.push_back(t);
        regexes.push_back("aba");
        tests.push_back({
            "aba",
            "bab",
            "baba",
            "ababababababababababab" });
        expects.push_back({
            {
                match(tests[t][0], 0, 3, true)},
            { },
            {
                match(tests[t][2], 1, 4, true)},
            {
                match(tests[t][3], 0, 3, true),
                match(tests[t][3], 4, 7, true),
                match(tests[t][3], 8, 11, true),
                match(tests[t][3], 12, 15, true),
                match(tests[t][3], 16, 19, true)}
            });
    }
    void test3() {
        int t = 3;
        test_numbers.push_back(t);
        regexes.push_back("NK((abc|ABC)*N|(OP)+)Q");
        tests.push_back({
            "NKNQ",
            "NKOPQ",
            "NK",
            "OPQ",
            "NKabcabcabcNQ",
            "oosoiduuufufffii",
            "NKABCABCNQ",
            "NKabcABCabcABCABCabcABCNQ",
            "NKOPOPOPOPQ",
            "sofieNKNQofisNOPQkfuNKOPOPQlzNKABCNQksofuNKabcNQtttat" });
        expects.push_back({
            {
                match(tests[t][0], 0, 4, true)},
            {
                match(tests[t][1], 0, 5, true)},
            { },
            { },
            {
                match(tests[t][4], 0, 13, true)},
            { },
            {
                match(tests[t][6], 0, 10, true)},
            {
                match(tests[t][7], 0, 25, true)},
            {
                match(tests[t][8], 0, 11, true)},
            {
                match(tests[t][9], 5, 9, true),
                match(tests[t][9], 20, 27, true),
                match(tests[t][9], 29, 36, true),
                match(tests[t][9], 41, 48, true)}
            });
    }
    void test4() {
        int t = 4;
        test_numbers.push_back(t);
        regexes.push_back("ab+c");
        tests.push_back({
            "a",
            "ac",
            "abc",
            "abbc",
            "abbbbbb",
            "abbbc",
            "abbbbbc",
            "abbbbababcccccabbbbcabbbbabacacabccccab" });
        expects.push_back({
            { },
            { },
            {
                match(tests[t][2], 0, 3, true)},
            {
                match(tests[t][3], 0, 4, true)},
            { },
            {
                match(tests[t][5], 0, 5, true)},
            {
                match(tests[t][6], 0, 7, true)},
            {
                match(tests[t][7], 7, 10, true),
                match(tests[t][7], 14, 20, true),
                match(tests[t][7], 31, 34, true)}
            });
    }
    void test5() {
        int t = 5;
        test_numbers.push_back(t);
        regexes.push_back("abc+");
        tests.push_back({
            "a",
            "aaa",
            "aaaabb",
            "aabbcc",
            "ab",
            "abc",
            "abcc",
            "abccc",
            "abcccccc",
            "kkkabcccccccciabccccccabccccckkkcccccabccccccccccco" });
        expects.push_back({
            { }, { }, { }, { }, { },
            {
                match(tests[t][5], 0, 3, true)},
            {
                match(tests[t][6], 0, 4, true)},
            {
                match(tests[t][7], 0, 5, true)},
            {
                match(tests[t][8], 0, 8, true)},
            {
                match(tests[t][9], 3, 13, true),
                match(tests[t][9], 14, 22, true),
                match(tests[t][9], 22, 29, true),
                match(tests[t][9], 37, 50, true)}
            });
    }
    void test6() {
        int t = 6;
        test_numbers.push_back(t);
        regexes.push_back("aB{3,6}c");
        tests.push_back({
            "ac",
            "aBc",
            "aBBc",
            "aBBBc",
            "aBBBBc",
            "aBBBBBc",
            "gaBBBBBBc",
            "ggaBBBBBBBc",
            "aBBBBBBBBc",
            "aBBBBBBBBBc",
            "aBBBBBBBBBBcg",
            "aBBBBBBBBBBBcgg",
            "aBBBBBBBBBBBBc",
            "aBBBBBBBBBBBBBc",
            "kaBBBcfaBBBBckkaBBckaBBBBBBcUUUaBBBBcOOOaBcTTTTaBBBBBcTTTTT" });
        expects.push_back({
            { }, { }, { },
            {
                match(tests[t][3], 0, 5, true)},
            {
                match(tests[t][4], 0, 6, true)},
            {
                match(tests[t][5], 0, 7, true)},
            {
                match(tests[t][6], 1, 9, true)},
            { }, { }, { }, { },
            { }, { }, { },
            {
                match(tests[t][14], 1, 6, true),
                match(tests[t][14], 7, 13, true),
                match(tests[t][14], 20, 28, true),
                match(tests[t][14], 31, 37, true),
                match(tests[t][14], 47, 54, true)}
            });
    }
    void test7() {
        int t = 7;
        test_numbers.push_back(t);
        regexes.push_back("jkT{4,5}");
        tests.push_back({
            "jkTT",
            "jkTTT",
            "jkTTTT",
            "jkTTTTT",
            "jkTTTTTT",
            "jkTTTTTTT" });
        expects.push_back({
            { },
            { },
            {
                match(tests[t][2], 0, 6, true)},
            {
                match(tests[t][3], 0, 7, true)},
            {
                match(tests[t][4], 0, 7, true)},
            {
                match(tests[t][5], 0, 7, true)}
            });
    }
    void test8() {
        int t = 8;
        test_numbers.push_back(t);
        regexes.push_back("T{3,5}");
        tests.push_back({
            "TT",
            "TTT",
            "TTTT",
            "TTTTT",
            "TTTTTT",
            "TTTTTTT" });
        expects.push_back({
            { },
            {
                match(tests[t][1], 0, 3, true)},
            {
                match(tests[t][2], 0, 4, true)},
            {
                match(tests[t][3], 0, 5, true)},
            { },
            { }
            });
    }
    void test9() {
        int t = 9;
        test_numbers.push_back(t);
        regexes.push_back("T+");
        tests.push_back({
            "k",
            "T",
            "TT",
            "TTT",
            "kkTTTT",
            "TTTTT",
            "kkkkTTTTTT",
            "TTTTTTT",
            "kkkkTTTTTTz" });
        expects.push_back({
            { },
            {
                match(tests[t][1], 0, 1, true)},
            {
                match(tests[t][2], 0, 2, true)},
            {
                match(tests[t][3], 0, 3, true)},
            {
                match(tests[t][4], 2, 6, true)},
            {
                match(tests[t][5], 0, 5, true)},
            {
                match(tests[t][6], 4, 10, true)},
            {
                match(tests[t][7], 0, 7, true)},
            {
                match(tests[t][8], 4, 10, true)}
            });
    }
    
};

int main() {
    unit_tester tester;
    tester.run();
    return 0;
}

}

int main() { assignment1::main(); }
