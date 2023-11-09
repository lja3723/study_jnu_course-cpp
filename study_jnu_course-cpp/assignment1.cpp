//�� ������ ���� https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
using namespace std;

namespace assignment1 {

class MySimpleRegex {
//�� Ŭ������ static �޼��常 �����Ƿ� �����ڸ� private�� �д�.
private: MySimpleRegex(); 
public:
    // ���ڿ����� ���ϰ� ��ġ�� ������ ǥ���Ѵ�.
    class matched {
    private:
        const int m_start, m_end;   //���� ���ڿ������� ������
        const string& m_ref;        //���� ���ڿ�
        const bool m_valid;         //��ü�� ��ȿ�� �� ����

    public:
        matched(const string& ref, int start, int end, bool valid) : 
            m_ref(ref), m_start(start), m_end(end), m_valid(valid) {}

        //�ش� ��ü�� ��ȿ�� �� ���� Ȯ�� (false�� ��ȿ���� ����)
        bool valid() const { return m_valid; }

        //������ ���ϴ� ���ڿ� ��ȯ
        string group() const { return m_ref.substr(m_start, m_end - m_start); }

        //������ ���۰�
        int start() const { return m_start; }

        //������ ����
        int end() const { return m_end; }

        //����
        pair<int, int> span() const { return pair<int, int>(m_start, m_end); }
    };
    
    //TODO: �Ľ� �� ���� ��� �����ϴ� �˰��� �����ؾ� ��
    //����ǥ������ �������� ��ü��, Ȯ���� ���ڿ��� �Է��ϸ� ��ġ ������ ��ȯ�Ѵ�.
    class compiled {
        friend class MySimpleRegex;
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

        //��带 ī��Ű�� ������ Ŭ����(�ñ״�ó)
        class node_ptr;

        //���� ���丶Ÿ�� �����ϴ� ���
        class node {
        private:
            string m_name; // state node name
            int m_suspended_active_count; // ����� active_count
            bool m_suspended_state; // for syncronization: ����� state
            int m_active_count; // accept�� �͹̳� ��忡�� �νĵ� ���ڿ� ���̸� �ǹ��Ѵ�.
            bool m_state;       // 0: deactive, 1: active
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
                m_suspended_active_count = m_active_count = 0;
                m_suspended_state = m_state = false;
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

            //�� ����� Ȱ��ȭ ī���� ���� ��ȯ�Ѵ�.
            //accepted�� true�� �� �� ���� �νĵ� ���ڿ��� ���̸� �ǹ��Ѵ�.
            int active_count() { return m_active_count; }
            
            //�� ��尡 ����Ű�� ��带 �߰��Ѵ�.
            void addNode(node_ptr* _next) { next.push_back(_next); }
            
            //for syncronization: �� ��带 Ȱ��ȭ�ϵ��� �䫊�Ѵ�. ���°��� ����ȴ�.
            //���� transfer_state�� ȣ���� ����� ���¸� ��忡 ��¥ �ݿ��Ѵ�.
            void request_active(int next_active_count) {
                m_suspended_active_count = next_active_count;
                m_suspended_state = true; 
            }
            
            //��� ���¿� ���õ� ���� �÷��׵��� ��� Ŭ�����Ѵ�.
            void clear_flags() {
                m_suspended_active_count = m_active_count = 0;
                m_suspended_state = m_state = false;
                m_isAccepted = false;
            }
           
            //�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
            bool is_accepted() {
                ///*debug*/ if (m_isAccepted) cout << "  >>>> accepted by " << m_name << endl;
                return m_isAccepted; //active ���°� �ƴµ� �͹̳��̸� accepted�� �Ǿ���(true ��ȯ)
            }
            
            //����� ���µ��� �� ���·� ���۽�Ų��.
            void transfer_state() { //���¸� ������
                m_active_count = m_suspended_active_count; //����� Ȱ�� ī���� ����
                m_state = m_suspended_state; //����� ���� ����

                m_suspended_active_count = 0;
                m_suspended_state = false;
            }

            //TODO: ���Խ� �� ���� A*�� �� ��� �����غ���
            //��忡 �� ���ڸ� �Է���: ��尡 Ȱ��ȭ���� �� ���ڰ� ��Ī�Ǹ� ���� ���鿡�� Ȱ��ȭ�� ��û��
            //��� Ȱ��ȭ & ���� ��Ī�� �� �͹̳� ����� ��� is_accepted�� true�� ��
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
        
        //��带 ����Ű�� ������(���� Ŭ����)
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
       
        //TODO: �־��� ����ǥ�������� ���¸ӽ� �����ϱ� �ʿ�
        compiled(const string& regex = "", int test_case = 0) {

            //mock implementation
            //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
            switch (test_case) {
                // abc|ade
            case 0:
                n_vec.resize(5/*����ǥ���� �ؼ� �� ��� ������ �Ǿ�� ��*/);

                n_vec[0] = new node("s0", 0, new match_single('a'));
                n_vec[1] = new node("s1", 0, new match_single('b'));
                n_vec[2] = new node("s2", 1, new match_single('c'));
                n_vec[3] = new node("s3", 0, new match_single('d'));
                n_vec[4] = new node("s4", 1, new match_single('e'));

                // set links
                n_vec[0]->addNode(new node_ptr_direct(n_vec[1]));
                n_vec[0]->addNode(new node_ptr_direct(n_vec[3]));
                n_vec[1]->addNode(new node_ptr_direct(n_vec[2]));
                n_vec[3]->addNode(new node_ptr_direct(n_vec[4]));
                break;

                // aba
            case 1:
                n_vec.resize(3);

                n_vec[0] = new node("s0", 0, new match_single('a'));
                n_vec[1] = new node("s1", 0, new match_single('b'));
                n_vec[2] = new node("s2", 1, new match_single('a'));

                n_vec[0]->addNode(new node_ptr_direct(n_vec[1]));
                n_vec[1]->addNode(new node_ptr_direct(n_vec[2]));
                break;

                // NK((abc|ABC)*N|(OP)+)Q
            case 2:
                n_vec.resize(12);

                n_vec[0] = new node("s0", 0, new match_single('N'));
                n_vec[1] = new node("s1", 0, new match_single('K'));
                n_vec[2] = new node("s2", 0, new match_single('O'));
                n_vec[3] = new node("s3", 0, new match_single('P'));
                n_vec[4] = new node("s4", 0, new match_single('a'));
                n_vec[5] = new node("s5", 0, new match_single('b'));
                n_vec[6] = new node("s6", 0, new match_single('c'));
                n_vec[7] = new node("s7", 0, new match_single('A'));
                n_vec[8] = new node("s8", 0, new match_single('B'));
                n_vec[9] = new node("s9", 0, new match_single('C'));
                n_vec[10] = new node("s10", 0, new match_single('N'));
                n_vec[11] = new node("s11", 1, new match_single('Q'));

                n_vec[0]->addNode(new node_ptr_direct(n_vec[1]));

                n_vec[1]->addNode(new node_ptr_direct(n_vec[2]));
                n_vec[1]->addNode(new node_ptr_direct(n_vec[4]));
                n_vec[1]->addNode(new node_ptr_direct(n_vec[7]));
                n_vec[1]->addNode(new node_ptr_direct(n_vec[10]));

                n_vec[2]->addNode(new node_ptr_direct(n_vec[3]));

                n_vec[3]->addNode(new node_ptr_direct(n_vec[2]));
                n_vec[3]->addNode(new node_ptr_direct(n_vec[11]));

                n_vec[4]->addNode(new node_ptr_direct(n_vec[5]));

                n_vec[5]->addNode(new node_ptr_direct(n_vec[6]));

                n_vec[6]->addNode(new node_ptr_direct(n_vec[4]));
                n_vec[6]->addNode(new node_ptr_direct(n_vec[10]));

                n_vec[7]->addNode(new node_ptr_direct(n_vec[8]));

                n_vec[8]->addNode(new node_ptr_direct(n_vec[9]));

                n_vec[9]->addNode(new node_ptr_direct(n_vec[7]));
                n_vec[9]->addNode(new node_ptr_direct(n_vec[10]));

                n_vec[10]->addNode(new node_ptr_direct(n_vec[11]));
                break;
            }
        }
        
        //�Ҹ��� (�޸� ��ȯ �ʿ�)
        ~compiled() {
            for (int i = 0; i < node_ptr_vector.size(); i++)
                if (node_ptr_vector[i] != nullptr)
                    delete node_ptr_vector[i];
        }

        //nodes
        vector<node *> n_vec;        
    public:

        //source���� ���� ó������ �߰ߵ� ��ġ ������ ��ȯ�Ѵ�.
        matched match(const string& source, int start_idx = 0) {
            int start = 0, end = 0;
            bool valid = false;

            ///*debug*/cout << "test for \"" << source << "\"" << endl;
            for (int i = start_idx; !valid && i < source.length(); i++) {
                ///*debug*/cout << "  >> input " << source[i] << endl;

                // epsilon activation for s0
                n_vec[0]->request_active(1);
                n_vec[0]->transfer_state();

                // give ch all nodes
                // nv[j]�� active ���¿��ٸ� �� ���� �� nv[j]���� ������ ȭ��ǥ��
                // ������ ������ �� ����Ű�� ����� transited�� Ȱ��ȭ�Ѵ�.
                for (int j = 0; j < n_vec.size(); j++) {
                    n_vec[j]->input(source[i]);
                    if (n_vec[j]->is_accepted()) {
                        ///*debug*/cout << "       active_count: " << nv[j].active_count() << endl;
                        valid = true;
                        end = i + 1;
                        start = end - n_vec[j]->active_count();
                        break;
                    }
                }

                // determine transited state
                // transited�� Ȱ��ȭ �� ��� ��带 active�� �����Ѵ�.
                for (int j = 0; j < n_vec.size(); j++)
                    n_vec[j]->transfer_state();
            }
            ///*debug*/cout << endl;
            for (int i = 0; i < n_vec.size(); i++) n_vec[i]->clear_flags(); //ã�� �� �� ��� �÷��� �ʱ�ȭ
            return matched(source, start, end, valid);
        }

        //source���� �߰ߵǴ� ��� ��ġ ������ ��ȯ�Ѵ�.
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

    //����ǥ������ �������� ��ü�� ��ȯ�Ѵ�.
    static compiled compile(const string& regex, int test_case = 0) {
        return compiled(regex, test_case);
    }
    
    //source���� ����ǥ���İ� ���� ���� ��ġ�ϴ� ����(��ȿ���� ���� ���� ����)�� ���Ѵ�.
    static matched match(const string& regex, const string& source) {
        return compiled(regex).match(source);
    }
    
    //source���� ����ǥ���İ� ��ġ�ϴ� ��� ������ ���Ѵ�. ��ġ������ ���� ��� �� vector�� ��ȯ�ȴ�.
    static vector<matched> match_all(const string regex, const string& source) {
        return compiled(regex).match_all(source);
    }
};

void run_test() {
    string mock_regex[] = {
        "abc|ade",
        "aba",
        "NK((abc|ABC)*N|(OP)+)Q"};
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
        "sofieNKNQofisNOPQkfuNKOPOPQlzNKABCNQksofuNKabcNQtttat" }};
    int test_count = sizeof(tests) / sizeof(*tests);

    for (int t = 0; t < test_count; t++) {
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
                for (int i = 0; i < len; i++) cout << "^";
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
