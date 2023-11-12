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
    static compiled compile(const string& m_regex, int test);    
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

    //������ ���۰�
    unsigned start() const { return m_start; }

    //������ ����
    unsigned end() const { return m_end; }

    //����
    pair<unsigned, unsigned> span() const { return pair<unsigned, unsigned>(m_start, m_end); }

    //������ �����ϴ� ���� ���ڿ� ���� ��ȯ
    const string& ref() const { return m_ref; }

    //������ ���ϴ� ���ڿ� ��ȯ
    string group() const { return m_ref.substr(m_start, m_end - m_start); }

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
    void create_state_machine(int test) {

        //mock implementation
        //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
        if (test == 1) {
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
        else if (test == 2) {
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
        else if (test == 3) {
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
        else if (test == 4) {
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
        else if (test == 5) {
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
        else if (test == 6) {
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
        else if (test == 7) {
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
        else if (test == 8) {
            //T{3,5}
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->addNode(new node_ptr_direct(new match_single('T'), m_node[1]));
            m_node[0]->addNode(new node_ptr_cnt_inner(new match_single('T'), m_node[1], 3, 5));

            m_get_epsilon.push_back(m_node[0]);
            m_terminal.push_back(m_node[1]);
        }
        else if (test == 9) {
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
            //���Ƿ� ��ȣ�� �� �ֱ�⸶�� �ο���
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
    compiled(const string& m_regex = "", int test = 0) : m_regex(m_regex) {
        create_state_machine(test);
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



MySimpleRegex::compiled MySimpleRegex::compile(const string& m_regex, int test = 0) {
    return compiled(m_regex, test);
}
MySimpleRegex::matched MySimpleRegex::match(const string& m_regex, const string& source) {
    return compiled(m_regex).match(source);
}
vector<MySimpleRegex::matched> MySimpleRegex::match_all(const string m_regex, const string& source) {
    return compiled(m_regex).match_all(source);
}



class MyUnitTest {
    using match = MySimpleRegex::matched;
    using matches = vector<match>;

private:
    struct test {
        int number;             //test case number
        string regex;           //regex of test
        vector<string> test;    //input strings for regex
        vector<matches> expect; //expect results
        bool details;           //0: less detail, 1: more detail
        bool enabled;           //0: test disabled, 1: test enabled
    };
    vector<test> tests;         //�׽�Ʈ ���
    int m_newline;              //������ �ʿ��� �� ���� ���� ��
    char m_underline_marker;    //���� ǥ���� ����
    string m_indent;            //�鿩���� ����
    int details_summary_mode;   //�׽�Ʈ ��� ǥ�ÿ���

    bool assertEqual(matches& expect, matches& result) {
        if (expect.size() != result.size()) return false;

        for (int i = 0; i < expect.size(); i++) {
            if (expect[i].start() != result[i].start()) return false;
            if (expect[i].end() != result[i].end())     return false;
            if (expect[i].valid() != result[i].valid()) return false;
        }
        return true;
    }

    vector<bool> run_test(int t) {
        vector<bool> ret;

        MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[t].regex, t);
        for (int i = 0; i < tests[t].test.size(); i++) {
            matches list_all = cp.match_all(tests[t].test[i]);
            ret.push_back(assertEqual(tests[t].expect[i], list_all));
        }

        return ret;
    }

    void run_tests() {
        //��Ȱ��ȭ�� �׽�Ʈ ����Ʈ ���ϱ�
        set<int> disabled; 
        for (int t = 1; t < tests.size(); t++)
            if (!tests[t].enabled) disabled.insert(tests[t].number);

        //�׽�Ʈ ��� ��� ���� ��� ��������
        vector<int> successed_list;
        vector<int> failed_list;

        //�� �׽�Ʈ ���̽��� ���� �׽�Ʈ ����
        for (int t = 1; t < tests.size(); t++) {
            if (disabled.find(t) != disabled.end()) continue; //disabled�� �׽�Ʈ ��ŵ
            vector<bool> results = run_test(tests[t].number); //�׽�Ʈ ����

            //�׽�Ʈ ��������
            bool test_successed = true;
            for (bool result : results) test_successed &= result;

            if (test_successed)
                print_successed_test(successed_list, t);
            else
                print_failed_test(failed_list, results, t);
            cout << "\n\n";
        }

        //�׽�Ʈ ���� ��� ���
        print_tests_summary(disabled, successed_list, failed_list);
    }

    void print_successed_test(vector<int>& successed_list, int test_number) {
        successed_list.push_back(test_number);
        if (!tests[test_number].details) {
            cout << "�׽�Ʈ #" << test_number << " ���" << endl;
            return;
        }

        print_test_title("�׽�Ʈ #", test_number, " ��� (more detail mode enabled)");
        MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex, test_number);

        for (int i = 0; i < tests[test_number].test.size(); i++) {
            vector<MySimpleRegex::matched> result = cp.match_all(tests[test_number].test[i]);
            cout << "case #" << (i + 1) << endl;
            print_elem_with_range("< ������ >", result, test_number, i);
        }
    }

    void print_failed_test(vector<int>& failed_list, vector<bool>& results, int test_number) {
        failed_list.push_back(test_number);
        MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex, test_number);
        bool title_printed = false;
        for (int i = 0; i < results.size(); i++) {
            if (!tests[test_number].details && results[i] == true) continue;
            matches result = cp.match_all(tests[test_number].test[i]);

            //������ �׽�Ʈ ���� ��� (���� 1ȸ��)
            if (!title_printed) {
                title_printed = true;
                if (!tests[i].details)
                    print_test_title("<!>>>>>>> �׽�Ʈ #", test_number, " ����");
                else
                    print_test_title("<!>>>>>>> �׽�Ʈ #", test_number, " ���� (more detail mode enabled)");
            }

            //������ ���̽� ���
            cout << "case #" << (i + 1) << (results[i] ? " (���)" : " (����)") << endl;
            if (!results[i])
                print_elem_with_range("< ����� >", tests[test_number].expect[i], test_number, i);
            print_elem_with_range("< ������ >", result, test_number, i);
        }
    }

    void print_tests_summary(set<int>& disabled, vector<int>& successed_list, vector<int>& failed_list) {
        if (details_summary_mode) {
            string line = "=========================";
            unsigned tests_size = unsigned(tests.size() - 1);
            cout << line << "  [�׽�Ʈ ���� ���]  " << line << endl;
            cout << "�� �׽�Ʈ���̽� ��:\t" << tests_size << " ��\n";
            if (!disabled.empty())
                cout << "��Ȱ��ȭ�� �׽�Ʈ ��:\t" << disabled.size() << " ��\n";
            cout << "������ �׽�Ʈ ��:\t" << (tests_size - disabled.size()) << " ��\n";
            cout << "������ �׽�Ʈ���̽� ��:\t" << (successed_list.size()) << " ��\n";
            if (!failed_list.empty())
                cout << "������ �׽�Ʈ���̽� ��:\t" << failed_list.size() << " ��\n";
            cout << endl;

            if (!disabled.empty()) {
                cout << "��Ȱ��ȭ�� �׽�Ʈ���̽� ���:" << endl << m_indent << m_indent;
                for (int test : disabled) cout << test << ";  ";
                cout << endl;
            }
            if (!disabled.empty()) {
                cout << "������ �׽�Ʈ���̽� ���:" << endl << m_indent << m_indent;
                for (int test : successed_list) cout << test << ";  ";
                cout << endl;
            }
            if (!failed_list.empty()) {
                cout << "������ �׽�Ʈ���̽� ���:" << endl << m_indent << m_indent;
                for (int test : failed_list) cout << test << ";  ";
                cout << endl;
            }
        }
        else {
            if (failed_list.empty()) {
                cout << "�׽�Ʈ�� ��� ����Ͽ����ϴ�." << endl;
                if (disabled.empty()) return;
                cout << m_indent << "��Ȱ��ȭ�� �׽�Ʈ ����Ʈ:  ";
                for (int test : disabled) cout << test << ";  ";
            }
            else {
                cout << "������ �׽�Ʈ�� �ֽ��ϴ�." << endl << m_indent << "����Ʈ:  ";
                for (int test : failed_list) cout << test << ";  ";
            }
            cout << endl;
        }
        cout << endl;
        
        //less_details_all�� Ȱ��ȭ���� Ȯ��
        bool less_details_all = true;
        for (int i = 1; i < tests.size(); i++) 
            less_details_all &= !tests[i].details;
        if (less_details_all) {
            cout << "����ȭ�� �׽�Ʈ ��� ���Ⱑ ���� �ֽ��ϴ�." << endl;
            cout << "�� �ڼ��� �׽�Ʈ ����� ���÷��� run() ���� ������ more_details_all()�� �����ϼ���." << endl;
        }
    }

    void print_test_title(string label_left, int test_number, string label_right) {
        cout << "\n\n\n";
        cout << label_left << test_number << " \"" << tests[test_number].regex << "\"" << label_right << endl;
    }

    void print_elem_with_range(string title, matches& match_result, int test_case, int elem) {
        cout << m_indent << title << endl;
        if (match_result.empty()) {
            cout << m_indent << tests[test_case].test[elem] << " -> ��ġ���� ����" << "\n\n";
            return;
        }

        print_match_info(match_result, m_indent);
        for (const MySimpleRegex::matched& mt : match_result) {
            cout << m_indent << m_indent;
            cout << "-> range [" << mt.start() << "," << mt.end() << ") " << "\"" << mt.group() << "\"\n";
        }
        cout << endl;
    }

    //��ġ�� ������ ���� ǥ��; �Է� ���ڿ��� ��� newline���� �����
    void print_match_info(const vector<MySimpleRegex::matched>& result, const string& indent) {
        if (result.empty()) return;
        const string& ref = result[0].ref();

        //���ڿ��� ������ ��� ���
        for (size_t i = 0, cur_match = 0; i < ref.size(); i += m_newline) {
            size_t len = i + m_newline <= ref.size() ? m_newline : ref.size() - i;
            cout << indent << ref.substr(i, len) << endl; //���� ���ڿ� ���  
            if (cur_match >= result.size() || i + len <= result[cur_match].start()) {
                cout << endl;
                continue;
            }

            cout << indent;
            for (size_t j = i; j < i + len; j++) {
                const MySimpleRegex::matched& match = result[cur_match];
                cout << (match.start() <= j && j < match.end() ? m_underline_marker : ' '); // ���� ���
                if (j + 1 >= match.end()) cur_match++;
                if (cur_match >= result.size()) break;
            }
            cout << endl;
        }
    }

public:
    MyUnitTest() : 
        m_newline(80), 
        m_underline_marker('^') {
        set_indent_level(3);
        set_summary_more_details();

        register_tests();
        enable_all().more_details_all();
    }
    void run() { run_tests(); }

    MyUnitTest& set_newline(int newline) {
        m_newline = newline;
        return *this;
    }
    MyUnitTest& set_underline_marker(char marker) {
        m_underline_marker = marker;
        return *this;
    }
    MyUnitTest& set_indent_level(int indent_level) {
        m_indent = "";
        for (int i = 0; i < indent_level; i++) m_indent += " ";
        return *this;
    }
    MyUnitTest& set_summary_more_details() {
        details_summary_mode = true;
        return *this;
    }
    MyUnitTest& set_summary_less_details() {
        details_summary_mode = false;
        return *this;
    }

    MyUnitTest& more_details(int test) { return more_details({ test }); }
    MyUnitTest& more_details(initializer_list<int> tests_list) {
        for (int test_num: tests_list) tests[test_num].details = true;
        return *this;
    }
    MyUnitTest& less_details(int test) { return less_details({ test }); }
    MyUnitTest& less_details(initializer_list<int> tests_list) {
        for (int test_num : tests_list) tests[test_num].details = false;
        return *this;
    }
    MyUnitTest& more_details_all() {
        for (test& t : tests) t.details = true;
        return *this;
    }
    MyUnitTest& less_details_all() {
        for (test& t : tests) t.details = false;
        return *this;
    }

    MyUnitTest& enable(int test) { return enable({ test }); }
    MyUnitTest& enable(initializer_list<int> tests_list) {
        cout << "initializer_list" << endl;
        for (int test_num : tests_list) tests[test_num].enabled = true;
        return *this;
    }
    MyUnitTest& disable(int test) { return disable({ test }); }
    MyUnitTest& disable(initializer_list<int> tests_list) {
        for (int test_num: tests_list) tests[test_num].enabled = false;
        return *this;
    }
    MyUnitTest& enable_all() {
        for (test& t : tests) t.enabled = true;
        return *this;
    }
    MyUnitTest& disable_all() {
        for (test& t : tests) t.enabled = false;
        return *this;
    }

protected:

    //������ �׽�Ʈ ���
    void register_tests() {
        test0(); //register example (<!> do not delete <!>)
        test1(); test2(); test3(); test4(); test5();
        test6(); test7(); test8(); test9();
    }

    //test func example
    void test0() {
        int t = 0;
        vector<string> test({
            "test_str",
            "test_str"});

        tests.push_back({
            t, "test_regex", test,
        {{
            match(test[0], 0, 0, true)},
        {
            match(test[1], 0, 0, true)}}});
    }
    
    void test1() {
        int t = 1;
        static vector<string> test({
            "abc",
            "ade",
            "aaaaabbbbabcaaaaaadeaaaaabc",
            "abbbbbbbbdddddddfdfdfdfdf",
            "zzosiduabcfdsfdafdadere",
            "aosisudababaifudiaddadeidufodi",
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" 
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" 
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" 
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" });
        
        tests.push_back({
            t,  "abc|ade", test,
        {{
            match(test[0], 0, 3, true)},
        {
            match(test[1], 0, 3, true) },
        {
            match(test[2], 9, 12, true),
            match(test[2], 17, 20, true),
            match(test[2], 24, 27, true) },
        { },
        {
            match(test[4], 7, 10, true),
            match(test[4], 18, 21, true), },
        {
            match(test[5], 20, 23, true) },
        {
            match(test[6], 23, 26, true),
            match(test[6], 27, 30, true),
            match(test[6], 100, 103, true),
            match(test[6], 107, 110, true)}}});    
    }
    void test2() {
        int t = 2;
        static vector<string> test({
            "aba",
            "bab",
            "baba",
            "ababababababababababab" });

        tests.push_back({
            t, "aba", test,
        {{
            match(test[0], 0, 3, true)},
        { },
        {
            match(test[2], 1, 4, true)},
        {
            match(test[3], 0, 3, true),
            match(test[3], 4, 7, true),
            match(test[3], 8, 11, true),
            match(test[3], 12, 15, true),
            match(test[3], 16, 19, true)}}});
    }
    void test3() {
        int t = 3;
        static vector<string> test({
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

        tests.push_back({
            t, "NK((abc|ABC)*N|(OP)+)Q", test,
        {{
            match(test[0], 0, 4, true)},
        {
            match(test[1], 0, 5, true)},
        { },
        { },
        {
            match(test[4], 0, 13, true)},
        { },
        {
            match(test[6], 0, 10, true)},
        {
            match(test[7], 0, 25, true)},
        {
            match(test[8], 0, 11, true)},
        {
            match(test[9], 5, 9, true),
            match(test[9], 20, 27, true),
            match(test[9], 29, 36, true),
            match(test[9], 41, 48, true)}}});
    }
    void test4() {
        int t = 4;
        static vector<string> test({
            "a",
            "ac",
            "abc",
            "abbc",
            "abbbbbb",
            "abbbc",
            "abbbbbc",
            "abbbbababcccccabbbbcabbbbabacacabccccab" });

        tests.push_back({
            t, "ab+c", test,
        {{ },
        { },
        {
            match(test[2], 0, 3, true)},
        {
            match(test[3], 0, 4, true)},
        { },
        {
            match(test[5], 0, 5, true)},
        {
            match(test[6], 0, 7, true)},
        {
            match(test[7], 7, 10, true),
            match(test[7], 14, 20, true),
            match(test[7], 31, 34, true)}}});
    }
    void test5() {
        int t = 5;
        static vector<string> test({
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

        tests.push_back({
            t, "abc+", test,
        {{ }, { }, { }, { }, { },
        {
            match(test[5], 0, 3, true)},
        {
            match(test[6], 0, 4, true)},
        {
            match(test[7], 0, 5, true)},
        {
            match(test[8], 0, 8, true)},
        {
            match(test[9], 3, 13, true),
            match(test[9], 14, 22, true),
            match(test[9], 22, 29, true),
            match(test[9], 37, 50, true)}}});
    }
    void test6() {
        int t = 6;
        static vector<string> test({
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

        tests.push_back({ 
            t, "aB{3,6}c", test,
        {{ }, { }, { },
        {
            match(test[3], 0, 5, true)},
        {
            match(test[4], 0, 6, true)},
        {
            match(test[5], 0, 7, true)},
        {
            match(test[6], 1, 9, true)},
        { }, { }, { }, { }, { }, { }, { },
        {
            match(test[14], 1, 6, true),
            match(test[14], 7, 13, true),
            match(test[14], 20, 28, true),
            match(test[14], 31, 37, true),
            match(test[14], 47, 54, true)}} });
    }
    void test7() {
        int t = 7;
        static vector<string> test({
            "jkTT",
            "jkTTT",
            "jkTTTT",
            "jkTTTTT",
            "jkTTTTTT",
            "jkTTTTTTT" });

        tests.push_back({
            t, "jkT{4,5}", test,
        {{ }, { },
        {
            match(test[2], 0, 6, true)},
        {
            match(test[3], 0, 7, true)},
        {
            match(test[4], 0, 7, true)},
        {
            match(test[5], 0, 7, true)}}});
    }
    void test8() {
        int t = 8;
        static vector<string> test({
            "TT",
            "TTT",
            "TTTT",
            "TTTTT",
            "TTTTTT",
            "TTTTTTT" });

        tests.push_back({
            t, "T{3,5}", test,
        {{ },
        {
            match(test[1], 0, 3, true)},
        {
            match(test[2], 0, 4, true)},
        {
            match(test[3], 0, 5, true)},
        { }, { }    }});
    }
    void test9() {
        int t = 9;
        static vector<string> test({
            "k",
            "T",
            "TT",
            "TTT",
            "kkTTTT",
            "TTTTT",
            "kkkkTTTTTT",
            "TTTTTTT",
            "kkkkTTTTTTz" });

        tests.push_back({
            t, "T+", test,
        {
        { },
        {
            match(test[1], 0, 1, true)},
        {
            match(test[2], 0, 2, true)},
        {
            match(test[3], 0, 3, true)},
        {
            match(test[4], 2, 6, true)},
        {
            match(test[5], 0, 5, true)},
        {
            match(test[6], 4, 10, true)},
        {
            match(test[7], 0, 7, true)},
        {
            match(test[8], 4, 10, true)}}});;
    }
    
};

int main() {
    MyUnitTest test;
    test.less_details_all().disable({1}).run();

    return 0;
}

}

int main() { assignment1::main(); }
