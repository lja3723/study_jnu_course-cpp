//�� ������ ���� https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
#include <map>
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



//TODO: �Ľ� �� ���� ��� �����ϴ� �˰��� �����ؾ� ��
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

    //��带 ����Ű�� ������(���� Ŭ����)
    class node_ptr {
    protected: node* m_pNode;
    public:
        static const unsigned INF = ~0U; //2^32 - 1
        node_ptr(node* pNode) : m_pNode(pNode) {}
        //����� ��忡 active�� ��û�Ѵ�. �ѹ��� �� ���� �ְ�, �׷��� ���� ���� ����
        virtual void transition_request(unsigned state_istart) = 0;
        virtual void transition_cancel() {} //������ ����
    };

    //��带 ���� �����ϴ� ������
    //���� ��û�� ������ ������ ���� ��ȣ ������
    class node_ptr_direct : public node_ptr {
    public:
        node_ptr_direct(node* pNode) : node_ptr(pNode) {}
        virtual void transition_request(unsigned state_istart) override {
            m_pNode->request_active(state_istart);
        }
    };

    //��带 ���Ǻη� �����ϴ� ������
    //���� ��û�� ���� Ƚ�� ���� ��쿡�� ���� ��ȣ ������
    class node_ptr_countable : public node_ptr {
    private:
        unsigned counter;
        unsigned lower_bound;
        unsigned upper_bound;
    public:
        node_ptr_countable(node* pNode, unsigned low_bound = 0, unsigned up_bound = node_ptr::INF)
            : node_ptr(pNode), counter(0), lower_bound(low_bound), upper_bound(up_bound) {}
        virtual void transition_request(unsigned state_istart) override {
            counter++;
            //������ ����� ��츸 transition�� �����Ѵ�
            if (lower_bound <= counter && counter <= upper_bound)
                m_pNode->request_active(state_istart);
        }
        virtual void transition_cancel() override {
            counter = 0;
        }
    };

    //���� ���丶Ÿ�� �����ϴ� ���
    class node {
    private:
        string m_name; // state node name
        unsigned m_state_istart_suspended; // ����� state_istart
        bool m_state_active_suspended; // for syncronization: ����� state

        unsigned m_state_istart;     // state�� ù active�Ǿ��� �� �ԷµǾ��� ������ �ε���
        bool m_state_active;    // 0: deactive, 1: active

        bool m_isTerminal;  // 0: terminal, 1: non-terminal
        bool m_isAccepted;  // 0: non-accept,   1: accept

        Imatchable* matcher; // target character for matching
        vector<node_ptr*> next; // next link 

    public:
        //�ݺ� �������� ��� �����ϴ�. ���Ѵ븦 �ǹ��Ѵ�.
        static const unsigned INF = unsigned(-1); //2^31 - 1

        //������
        node(string name = "", bool isTerminal = false, Imatchable* match = nullptr) :
            m_name(name), m_isTerminal(isTerminal), matcher(match), next(0) {
            m_state_istart_suspended = m_state_istart = 0;
            m_state_active_suspended = m_state_active = false;
            m_isAccepted = false;
        }

        //�Ҹ���: new�� �Ҵ���� �޸𸮸� ��ȯ�Ѵ�.
        ~node() {
            if (matcher != nullptr)
                delete matcher;

            for (int i = 0; i < next.size(); i++)
                if (next[i] != nullptr)
                    delete next[i];
        }

        //ó������ Ȱ��ȭ�� ���¿��� �� ù �ε����� ��ȯ�Ѵ�.
        //is_accepted�� �� �νĵ� ���ڿ��� ù ��ġ�� �ǹ��Ѵ�.
        unsigned index_start() { return m_state_istart; }

        //�� ��尡 ����Ű�� ��带 �߰��Ѵ�.
        void addNode(node_ptr* _next) { next.push_back(_next); }

        //for syncronization: �� ��带 Ȱ��ȭ�ϵ��� �䫊�Ѵ�. ���°��� ����ȴ�.
        //���� transfer_state�� ȣ���� ����� ���¸� ��忡 ��¥ �ݿ��Ѵ�.
        void request_active(unsigned state_istart) {
            m_state_istart_suspended = state_istart;
            m_state_active_suspended = true;
        }

        //��� ���¿� ���õ� ���� �÷��׵��� ��� Ŭ�����Ѵ�.
        void clear_flags() {
            m_state_istart_suspended = m_state_istart = 0;
            m_state_active_suspended = m_state_active = false;
            m_isAccepted = false;
        }

        //�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
        bool is_accepted() {
            ///*debug*/ if (m_isAccepted) cout << "  >>>> accepted by " << m_name << endl;
            return m_isAccepted; //active ���°� �ƴµ� �͹̳��̸� accepted�� �Ǿ���(true ��ȯ)
        }

        //����� ���µ��� �� ���·� ���۽�Ų��.
        void transfer_state() { //���¸� ������
            m_state_istart = m_state_istart_suspended; //����� Ȱ�� ī���� ����
            m_state_active = m_state_active_suspended; //����� ���� ����

            m_state_istart_suspended = 0;
            m_state_active_suspended = false;
        }

        //TODO: ���Խ� �� ���� A*�� �� ��� �����غ���
        //��忡 �� ���ڸ� �Է���: ��尡 Ȱ��ȭ���� �� ���ڰ� ��Ī�Ǹ� ���� ���鿡�� Ȱ��ȭ�� ��û��
        //��� Ȱ��ȭ & ���� ��Ī�� �� �͹̳� ����� ��� is_accepted�� true�� ��
        void input(const char& ch) {
            bool match = matcher->test(ch);

            for (int i = 0; i < next.size(); i++) {
                if (m_state_active && match)
                    next[i]->transition_request(m_state_istart);
                else
                    next[i]->transition_cancel();
            }

            //�νĵǾ��� ��� true
            m_isAccepted = m_state_active && match && m_isTerminal;
            m_state_active = false; // deactivate this node after matching
        }
    };



    /****************************/
    /*      Private fields      */
    /****************************/
    vector<node*> m_node; //���±�� ��� �����̳�
    vector<node*> m_get_epsilon; //���Ƿ� ��ȣ�� �޴� ��帮��Ʈ
    const string& m_regex; //����� ����ǥ����



    /****************************/
    /*    Private functions     */
    /****************************/

    //TODO: �־��� ����ǥ�������� ���¸ӽ� �����ϱ� �ʿ�
    //���±�� ��� �����̳ʿ� ���±�踦 �����Ѵ�.
    void create_state_machine(int test_case) {

        //mock implementation
        //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
        if (test_case == 0) {
            // abc|ade
            m_node.resize(5/*����ǥ���� �ؼ� �� ��� ������ �Ǿ�� ��*/);

            m_node[0] = new node("s0", 0, new match_single('a'));
            m_node[1] = new node("s1", 0, new match_single('b'));
            m_node[2] = new node("s2", 1, new match_single('c'));
            m_node[3] = new node("s3", 0, new match_single('d'));
            m_node[4] = new node("s4", 1, new match_single('e'));

            // set links
            m_node[0]->addNode(new node_ptr_direct(m_node[1]));
            m_node[0]->addNode(new node_ptr_direct(m_node[3]));
            m_node[1]->addNode(new node_ptr_direct(m_node[2]));
            m_node[3]->addNode(new node_ptr_direct(m_node[4]));

            // ���Ƿ� ��ȣ �޴� ��� ����
            m_get_epsilon.push_back(m_node[0]);
        }
        else if (test_case == 1) {
            // aba
            m_node.resize(3);

            m_node[0] = new node("s0", 0, new match_single('a'));
            m_node[1] = new node("s1", 0, new match_single('b'));
            m_node[2] = new node("s2", 1, new match_single('a'));

            m_node[0]->addNode(new node_ptr_direct(m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(m_node[2]));

            m_get_epsilon.push_back(m_node[0]);
        }
        else if (test_case == 2) {
            // NK((abc|ABC)*N|(OP)+)Q
            m_node.resize(12);

            m_node[0] = new node("s0", 0, new match_single('N'));
            m_node[1] = new node("s1", 0, new match_single('K'));
            m_node[2] = new node("s2", 0, new match_single('O'));
            m_node[3] = new node("s3", 0, new match_single('P'));
            m_node[4] = new node("s4", 0, new match_single('a'));
            m_node[5] = new node("s5", 0, new match_single('b'));
            m_node[6] = new node("s6", 0, new match_single('c'));
            m_node[7] = new node("s7", 0, new match_single('A'));
            m_node[8] = new node("s8", 0, new match_single('B'));
            m_node[9] = new node("s9", 0, new match_single('C'));
            m_node[10] = new node("s10", 0, new match_single('N'));
            m_node[11] = new node("s11", 1, new match_single('Q'));

            m_node[0]->addNode(new node_ptr_direct(m_node[1]));

            m_node[1]->addNode(new node_ptr_direct(m_node[2]));
            m_node[1]->addNode(new node_ptr_direct(m_node[4]));
            m_node[1]->addNode(new node_ptr_direct(m_node[7]));
            m_node[1]->addNode(new node_ptr_direct(m_node[10]));

            m_node[2]->addNode(new node_ptr_direct(m_node[3]));

            m_node[3]->addNode(new node_ptr_direct(m_node[2]));
            m_node[3]->addNode(new node_ptr_direct(m_node[11]));

            m_node[4]->addNode(new node_ptr_direct(m_node[5]));

            m_node[5]->addNode(new node_ptr_direct(m_node[6]));

            m_node[6]->addNode(new node_ptr_direct(m_node[4]));
            m_node[6]->addNode(new node_ptr_direct(m_node[10]));

            m_node[7]->addNode(new node_ptr_direct(m_node[8]));

            m_node[8]->addNode(new node_ptr_direct(m_node[9]));

            m_node[9]->addNode(new node_ptr_direct(m_node[7]));
            m_node[9]->addNode(new node_ptr_direct(m_node[10]));

            m_node[10]->addNode(new node_ptr_direct(m_node[11]));

            m_get_epsilon.push_back(m_node[0]);
        }
        else if (test_case == 3) {
            // ab+c
            m_node.resize(3);

            m_node[0] = new node("s0", 0, new match_single('a'));
            m_node[1] = new node("s1", 0, new match_single('b'));
            m_node[2] = new node("s2", 1, new match_single('c'));

            m_node[0]->addNode(new node_ptr_direct(m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(m_node[2]));

            m_get_epsilon.push_back(m_node[0]);
        }
        else if (test_case == 4) {
            // abc+
            m_node.resize(3);

            m_node[0] = new node("s0", 0, new match_single('a'));
            m_node[1] = new node("s1", 0, new match_single('b'));
            m_node[2] = new node("s2", 1, new match_single('c'));

            m_node[0]->addNode(new node_ptr_direct(m_node[1]));
            m_node[1]->addNode(new node_ptr_direct(m_node[2]));
            m_node[2]->addNode(new node_ptr_direct(m_node[2]));

            m_get_epsilon.push_back(m_node[0]);
        }
    }

    //������ ���±�踦 �����Ѵ�.
    void delete_state_machine() {
        for (int i = 0; i < m_node.size(); i++)
            if (m_node[i] != nullptr)
                delete m_node[i];
    }

    //���±�迡 ���ڿ��� �Է����� �ް�, ���� ó������ accept�� ��ġ ������ ����Ѵ�.
    matched state_machine_input(const string& src, unsigned index_start = 0, bool check_at_front_only = false) {
        map<int, matched*> found;

        ///*debug*/cout << "test for \"" << source << "\"" << endl;
        for (unsigned i = index_start; i < src.length(); i++) {
            ///*debug*/cout << "  >> input " << src[i] << endl;
            // epsilon activation for s0

            //check_at_front_only�� ���ڿ� ������ ���ϰ� ��ġ�ϴ� ��츸 Ȯ���Ѵ�.
            //�� ���� 1ȸ�� ���Ƿ� ��ȣ�� �ο��Ѵ�.
            if (!check_at_front_only || i == 0) {
                for (node* target : m_get_epsilon) {
                    target->request_active(i);
                        target->transfer_state();
                }
            }

            // give ch all nodes
            // nv[j]�� active ���¿��ٸ� �� ���� �� nv[j]���� ������ ȭ��ǥ��
            // ������ ������ �� ����Ű�� ����� transited�� Ȱ��ȭ�Ѵ�.
            for (node* target : m_node) {
                target->input(src[i]);
                if (target->is_accepted()) {
                    ///*debug*/cout << "       active_count: " << nv[j].active_count() << endl;
                    unsigned istart = target->index_start();
                    if (found.find(istart) != found.end()) delete found[istart];
                    found[istart] = new matched(src, istart, i + 1, true);
                }
            }

            // determine transited state
            // transited�� Ȱ��ȭ �� ��� ��带 active�� �����Ѵ�.
            for (node* target : m_node)
                target->transfer_state();
        }
        
        ///*debug*/cout << endl;
        //src�� ��� input�� �� ��� ����� �÷��� �ʱ�ȭ
        for (node* target : m_node) target->clear_flags();

        if (found.empty())
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



void run_test() {
    string mock_regex[] = {
        "abc|ade",
        "aba",
        "NK((abc|ABC)*N|(OP)+)Q",
        "ab+c",
        "abc+" };
    vector<string> tests[] = {
    {
        "abc",
        "ade",
        "aaaaabbbbabcaaaaaadeaaaaabc",
        "abbbbbbbbdddddddfdfdfdfdf",
        "zzosiduabcfdsfdafdadere",
        "aosisudababaifudiaddadeidufodi",
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" },
    {
        "aba",
        "bab",
        "baba",
        "ababababababababababab" },
    {
        "NKNQ",
        "NKOPQ",
        "NK",
        "OPQ",
        "NKabcabcabcNQ",
        "oosoiduuufufffii",
        "NKABCABCNQ",
        "NKabcABCabcABCABCabcABCNQ",
        "NKOPOPOPOPQ",
        "sofieNKNQofisNOPQkfuNKOPOPQlzNKABCNQksofuNKabcNQtttat" },
    {
        "a",
        "ac",
        "abc",
        "abbc",
        "abbbbbb",
        "abbbc",
        "abbbbbc",
        "abbbbababcccccabbbbcabbbbabacacabccccab" },
    {
        "a",
        "aaa",
        "aaaabb",
        "aabbcc",
        "ab",
        "abc",
        "abcc",
        "abccc",
        "abcccccc",
        "kkkabcccccccciabccccccabccccckkkcccccabccccccccccco"
    } };

    int test_enabled[] = { 0, 1, 2, 3, 4 };

    int test_count = sizeof(tests) / sizeof(*tests);
    int test_enabled_count = sizeof(test_enabled) / sizeof(*test_enabled);

    for (int te = 0; te < test_enabled_count; te++) {
        int t = test_enabled[te];
        cout << "****************************************************************\n";
        cout << "<�׽�Ʈ #" << (t + 1) << " / ����ǥ����: " << mock_regex[t] << ">" << endl;

        MySimpleRegex::compiled cp = MySimpleRegex::compile(mock_regex[t], t);
        for (int i = 0; i < tests[t].size(); i++) {
            vector<MySimpleRegex::matched> result = cp.match_all(tests[t][i]);
            cout << "\"" << tests[t][i] << "\"";
            if (result.empty()) {
                cout << " -> ��ġ���� ����" << endl;
                continue;
            }
            cout << endl;

            //��ġ�� ������ ���� ǥ��
            cout << " ";
            int last_print = 0;
            for (const MySimpleRegex::matched& mt : result) {
                int len = mt.start() - last_print;
                for (int i = 0; i < len; i++) cout << " ";
                len = mt.end() - mt.start();
                for (int i = 0; i < len; i++) cout << "*";
                last_print = mt.end();
            }
            cout << endl;
            

            for (const MySimpleRegex::matched& mt : result)
                cout << "    <���� [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";
        }
        cout << "\n\n";
    }
}

int main() { run_test(); return 0; }

}

int main() { assignment1::main(); }
