//�� ������ ���� https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
using namespace std;

namespace assignment1 {

class regex {
//�� Ŭ������ static �޼��常 �����Ƿ� �����ڸ� private�� �д�.
private: regex(); 
public:
    //description: ���ڿ����� ���ϰ� ��ġ�� ���� ��ȯ
    class matched {
    private:
        const int m_start, m_end;
        const string& m_ref;
        const bool m_valid;

    public:
        matched(const string& ref, int start, int end, bool valid) : 
            m_ref(ref), m_start(start), m_end(end), m_valid(valid) {}

        bool valid() const { return m_valid; }
        string group() const { return m_ref.substr(m_start, m_end - m_start); }
        int start() const { return m_start; }
        int end() const { return m_end; }
        pair<int, int> span() const { return pair<int, int>(m_start, m_end); }
    };
    
    //TODO: �Ľ� �� ���� ��� �����ϴ� �˰��� �����ؾ� �� / description: 
    class compiled {
        friend class regex;
    private:
        //��ġ ��ü �������̽�
        class Imatchable { public: virtual bool test(char ch) = 0; };
        class match_single : public Imatchable { //���� ���� ��ġ
        private: char ch;
        public: 
            match_single(char ch) : ch(ch) {}
            virtual bool test(char _ch) override { return ch == _ch; }
        };
        class match_dot : public Imatchable { //��� ���� ��ġ
        public:
            virtual bool test(char _ch) override {
                return ('A' <= _ch && _ch <= 'Z') || ('a' <= _ch && _ch <= 'z');
            }
        };

        class node_ptr;
        //TODO: ���� ���ϱ� ���� ������ ��� ���� �߰��ؾ� ��
        class node {
        private:
            string m_name; // state node name
            int m_suspended_active_count;
            bool m_suspended_state; // for syncronization
            int m_active_count; // �ش� ����� Ȱ��ȭ ī����: next node�鿡�� +1�� ���·� ���̵�
            bool m_state;
            bool m_isTerminal; // 0: terminal, 1: non-terminal
            bool m_isAccepted;

            Imatchable* matcher; // target character for matching
            vector<node_ptr*> next; // next link 

        public:
            static const unsigned INF = -(1U); //2^31 - 1
            node(string name = "", bool isTerminal = false, Imatchable* match = nullptr) :
                m_name(name), m_isTerminal(isTerminal), matcher(match), next(0) {
                m_suspended_active_count = m_active_count = 0;
                m_suspended_state = m_state = false;
                m_isAccepted = false;
            }
            node(node&& n) noexcept { *this = std::move(n); }
            node& operator=(node&& n) noexcept {
                ///*debug*/cout << "�̵� ���� ������ ȣ��" << endl;
                m_name = n.m_name;
                m_suspended_active_count = n.m_suspended_active_count;
                m_suspended_state = n.m_suspended_state;
                m_active_count = n.m_active_count;
                m_state = n.m_state;
                m_isTerminal = n.m_isTerminal;
                m_isAccepted = n.m_isAccepted;
                matcher = n.matcher;
                next = n.next;
                n.matcher = nullptr;

                return *this;
            }
            ~node() { 
                if (matcher != nullptr) 
                    delete matcher; 
                
                for (int i = 0; i < next.size(); i++) {
                    if (next[i] != nullptr)
                        delete next[i];
                }
            }

            int active_count() { return m_active_count; }
            void addNode(node_ptr* _next) { next.push_back(_next); }
            // for syncronization
            void request_active(int next_active_count) {
                m_suspended_active_count = next_active_count;
                m_suspended_state = true; 
            }
            void clear_flags() {
                m_suspended_active_count = m_active_count = 0;
                m_suspended_state = m_state = false;
                m_isAccepted = false;
            }
            bool is_accepted() {
                ///*debug*/ if (m_isAccepted) cout << "  >>>> accepted by " << m_name << endl;
                return m_isAccepted; //active ���°� �ƴµ� �͹̳��̸� accepted�� �Ǿ���(true ��ȯ)
            }
            //TODO: �ٽ� ������ ���� ����
            void transfer_state() { //���¸� ������
                m_active_count = m_suspended_active_count; //����� Ȱ�� ī���� ����
                m_state = m_suspended_state; //����� ���� ����

                m_suspended_active_count = 0;
                m_suspended_state = false;
            }
            //TODO:
            void input(const char& ch) {
                bool match = matcher->test(ch);

                for (int i = 0; i < next.size(); i++) {
                    if (m_state && match)
                        next[i]->transition_request(m_active_count + 1);
                    else
                        next[i]->transition_cancel();
                }

                //�νĵǾ��� ��� true
                m_isAccepted = m_state && match && m_isTerminal;                
                m_state = false; // deactivate this node after matching
            }
        };    
        class node_ptr { //��� ����Ű�� ������
        protected: node* m_pNode;
        public:
            static const unsigned INF = ~0U; //2^32 - 1
            node_ptr(node* pNode) : m_pNode(pNode) {}
            //����� ��忡 active�� ��û�Ѵ�. �ѹ��� �� ���� �ְ�, �׷��� ���� ���� ����
            virtual void transition_request(int next_active_count) = 0;       
            virtual void transition_cancel() {} //������ ����
        };
        class node_ptr_direct : public node_ptr {
        public:
            node_ptr_direct(node* pNode) : node_ptr(pNode) {}
            virtual void transition_request(int next_active_count) override { 
                m_pNode->request_active(next_active_count);
            }
        };
        class node_ptr_countable : public node_ptr {
        private: 
            unsigned counter;
            unsigned lower_bound;
            unsigned upper_bound;
        public:
            node_ptr_countable(node* pNode, unsigned low_bound = 0, unsigned up_bound = node_ptr::INF)
                : node_ptr(pNode), counter(0), lower_bound(low_bound), upper_bound(up_bound) {}
            virtual void transition_request(int next_active_count) override {
                counter++;
                //������ ����� ��츸 transition�� �����Ѵ�
                if (lower_bound <= counter && counter <= upper_bound)
                    m_pNode->request_active(next_active_count);
            }
            virtual void transition_cancel() override {
                counter = 0;
            }
        };

        vector<node> nv;

    public:
        //TODO: �־��� ����ǥ�������� ���¸ӽ� ����
        compiled(const string& regex = "", int test_case = 0) {

            //mock implementation
            //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
            switch (test_case) {
            // abc|ade
            case 0: 
                nv.resize(5/*����ǥ���� �ؼ� �� ��� ������ �Ǿ�� ��*/);

                nv[0] = node("s0", 0, new match_single('a'));
                nv[1] = node("s1", 0, new match_single('b'));
                nv[2] = node("s2", 1, new match_single('c'));
                nv[3] = node("s3", 0, new match_single('d'));
                nv[4] = node("s4", 1, new match_single('e'));

                // set links
                nv[0].addNode(new node_ptr_direct(&nv[1]));
                nv[0].addNode(new node_ptr_direct(&nv[3]));
                nv[1].addNode(new node_ptr_direct(&nv[2]));
                nv[3].addNode(new node_ptr_direct(&nv[4]));
                break;

            // aba
            case 1:
                nv.resize(3);

                nv[0] = node("s0", 0, new match_single('a'));
                nv[1] = node("s1", 0, new match_single('b'));
                nv[2] = node("s2", 1, new match_single('a'));

                nv[0].addNode(new node_ptr_direct(&nv[1]));
                nv[1].addNode(new node_ptr_direct(&nv[2]));
                break;

            // NK((abc|ABC)*N|(OP)+)Q
            case 2:
                nv.resize(12);

                nv[0] = node("s0", 0, new match_single('N'));
                nv[1] = node("s1", 0, new match_single('K'));
                nv[2] = node("s2", 0, new match_single('O'));
                nv[3] = node("s3", 0, new match_single('P'));
                nv[4] = node("s4", 0, new match_single('a'));
                nv[5] = node("s5", 0, new match_single('b'));
                nv[6] = node("s6", 0, new match_single('c'));
                nv[7] = node("s7", 0, new match_single('A'));
                nv[8] = node("s8", 0, new match_single('B'));
                nv[9] = node("s9", 0, new match_single('C'));
                nv[10] = node("s10", 0, new match_single('N'));
                nv[11] = node("s11", 1, new match_single('Q'));

                nv[0].addNode(new node_ptr_direct(&nv[1]));

                nv[1].addNode(new node_ptr_direct(&nv[2]));
                nv[1].addNode(new node_ptr_direct(&nv[4]));
                nv[1].addNode(new node_ptr_direct(&nv[7]));
                nv[1].addNode(new node_ptr_direct(&nv[10]));

                nv[2].addNode(new node_ptr_direct(&nv[3]));

                nv[3].addNode(new node_ptr_direct(&nv[2]));
                nv[3].addNode(new node_ptr_direct(&nv[11]));

                nv[4].addNode(new node_ptr_direct(&nv[5]));

                nv[5].addNode(new node_ptr_direct(&nv[6]));

                nv[6].addNode(new node_ptr_direct(&nv[4]));
                nv[6].addNode(new node_ptr_direct(&nv[10]));

                nv[7].addNode(new node_ptr_direct(&nv[8]));

                nv[8].addNode(new node_ptr_direct(&nv[9]));

                nv[9].addNode(new node_ptr_direct(&nv[7]));
                nv[9].addNode(new node_ptr_direct(&nv[10]));

                nv[10].addNode(new node_ptr_direct(&nv[11]));
                break;
            }
        }

        //���� ó������ �߰ߵ� ��ġ ������ ��ȯ�Ѵ�.
        matched match(const string& source, int start_idx = 0) {
            int start = 0, end = 0;
            bool valid = false;

            ///*debug*/cout << "test for \"" << source << "\"" << endl;
            for (int i = start_idx; !valid && i < source.length(); i++) {
                ///*debug*/cout << "  >> input " << source[i] << endl;

                // epsilon activation for s0
                nv[0].request_active(1);
                nv[0].transfer_state();

                // give ch all nodes
                // nv[j]�� active ���¿��ٸ� �� ���� �� nv[j]���� ������ ȭ��ǥ��
                // ������ ������ �� ����Ű�� ����� transited�� Ȱ��ȭ�Ѵ�.
                for (int j = 0; j < nv.size(); j++) {
                    nv[j].input(source[i]);
                    if (nv[j].is_accepted()) {
                        ///*debug*/cout << "       active_count: " << nv[j].active_count() << endl;
                        valid = true;
                        end = i + 1;
                        start = end - nv[j].active_count();
                        break;
                    }
                }

                // determine transited state
                // transited�� Ȱ��ȭ �� ��� ��带 active�� �����Ѵ�.
                for (int j = 0; j < nv.size(); j++)
                    nv[j].transfer_state();
            }
            ///*debug*/cout << endl;
            for (int i = 0; i < nv.size(); i++) nv[i].clear_flags(); //ã�� �� �� ��� �÷��� �ʱ�ȭ
            return matched(source, start, end, valid);
        }

        //�־��� ���ڿ����� ��ġ�Ǵ� ��� ��ġ ������ ��ȯ�Ѵ�.
        vector<matched> match_all(const string& source) {
            vector<matched> ret;
            int found_idx = 0;
            while (found_idx < source.size()) {
                matched ans = match(source, found_idx);
                if (!ans.valid()) break;
                ret.push_back(ans);
                found_idx = ret.back().end();
            }
            return ret;
        }
    };   

public:
    static compiled compile(const string& regex, int test_case = 0) {
        return compiled(regex, test_case);
    }
    static matched match(const string& regex, const string& source) {
        return compiled(regex).match(source);
    }
    static vector<matched> match_all(const string regex, const string& source) {
        return compiled(regex).match_all(source);
    }
};


void print_result(const vector<regex::matched>& result, string& test, string& regex) {
    cout << "\n���ڿ� \"" << test << "\"���� \"" << regex << "\"���ϰ� ��ġ�ϴ� �κ��� ";
    cout << (!result.empty() ? "�ֽ��ϴ�." : "�����ϴ�.") << endl;
    if (result.empty()) return;

    for (const regex::matched& mt : result) {
        cout << "���ϰ� ��ġ�ϴ� ���ڿ��� \"" << mt.group() << "\"�̰�, "
            << "[" << mt.start() << "," << mt.end() << ") ������ ���մϴ�.\n";
    }
}

int main() {
    string mock_regex[] = {
        "abc|ade",
        "aba",
        "NK((abc|ABC)*N|(OP)+)Q"
    };
    vector<string> tests[] = {
        {
            "abc",
            "ade",
            "aaaaabbbbabcaaaaaadeaaaaabc",
            "abbbbbbbbdddddddfdfdfdfdf",
            "zzosiduabcfdsfdafdadere",
            "aosisudababaifudiaddadeidufodi"
        },
        {
            "aba",
            "bab",
            "baba",
            "ababababababababababab"
        },
        {
            "NKNQ",
            "NKOPQ",
            "NK",
            "OPQ",

            "NKabcabcabcNQ",
            "oosoiduuufufffii",
            "NKABCABCNQ",
            "NKOPOPOPOPQ",

            "sofieNKNQofisNOPQkfuNKOPOPQlzNKABCNQksofuNKabcNQtttat"
        }
    };
    int test_size = sizeof(tests) / sizeof(*tests);

    for (int test = 0; test < test_size; test++) {
        cout << "\n\n\n<<< �׽�Ʈ #" << test << " >>>" << endl;
        regex::compiled cp = regex::compile(mock_regex[test], test);
        for (int i = 0; i < tests[test].size(); i++)
            print_result(cp.match_all(tests[test][i]), tests[test][i], mock_regex[test]);
    }

    return 0;
}



}

int main() { assignment1::main(); }
