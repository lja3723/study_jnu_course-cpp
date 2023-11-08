//깃 컨벤션 참고 https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
using namespace std;

namespace assignment1 {

class regex {
//이 클래스는 static 메서드만 있으므로 생성자를 private로 둔다.
private: regex(); 
public:
    //description: 문자열에서 패턴과 일치된 범위 반환
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
    
    //TODO: 파싱 후 상태 기계 생성하는 알고리즘 구현해야 됨 / description: 
    class compiled {
        friend class regex;
    private:
        //매치 객체 인터페이스
        class Imatchable { public: virtual bool test(char ch) = 0; };
        class match_single : public Imatchable { //단일 문자 매치
        private: char ch;
        public: 
            match_single(char ch) : ch(ch) {}
            virtual bool test(char _ch) override { return ch == _ch; }
        };
        class match_dot : public Imatchable { //모든 문자 매치
        public:
            virtual bool test(char _ch) override {
                return ('A' <= _ch && _ch <= 'Z') || ('a' <= _ch && _ch <= 'z');
            }
        };

        class node_ptr;
        //TODO: 길이 구하기 위해 적절한 멤버 변수 추가해야 됨
        class node {
        private:
            string m_name; // state node name
            int m_suspended_active_count;
            bool m_suspended_state; // for syncronization
            int m_active_count; // 해당 노드의 활성화 카운터: next node들에게 +1된 상태로 전이됨
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
                ///*debug*/cout << "이동 대입 연산자 호출" << endl;
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
                return m_isAccepted; //active 상태가 됐는데 터미널이면 accepted가 되었음(true 반환)
            }
            //TODO: 다시 봐야할 수도 있음
            void transfer_state() { //상태를 전이함
                m_active_count = m_suspended_active_count; //계류된 활성 카운터 전달
                m_state = m_suspended_state; //계류된 상태 전달

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

                //인식되었을 경우 true
                m_isAccepted = m_state && match && m_isTerminal;                
                m_state = false; // deactivate this node after matching
            }
        };    
        class node_ptr { //노드 가리키는 포인터
        protected: node* m_pNode;
        public:
            static const unsigned INF = ~0U; //2^32 - 1
            node_ptr(node* pNode) : m_pNode(pNode) {}
            //연결된 노드에 active를 요청한다. 한번에 될 수도 있고, 그렇지 않을 수도 있음
            virtual void transition_request(int next_active_count) = 0;       
            virtual void transition_cancel() {} //선택적 구현
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
                //범위에 들었을 경우만 transition을 전달한다
                if (lower_bound <= counter && counter <= upper_bound)
                    m_pNode->request_active(next_active_count);
            }
            virtual void transition_cancel() override {
                counter = 0;
            }
        };

        vector<node> nv;

    public:
        //TODO: 주어진 정규표현식으로 상태머신 생성
        compiled(const string& regex = "", int test_case = 0) {

            //mock implementation
            //문법을 해석해 아래 작업이 알아서 되어야함
            switch (test_case) {
            // abc|ade
            case 0: 
                nv.resize(5/*정규표현식 해석 후 노드 개수가 되어야 함*/);

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

        //가장 처음으로 발견된 매치 정보를 반환한다.
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
                // nv[j]가 active 상태였다면 이 연산 후 nv[j]에서 나오는 화살표는
                // 조건을 만족할 시 가리키는 노드의 transited를 활성화한다.
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
                // transited가 활성화 된 모든 노드를 active로 변경한다.
                for (int j = 0; j < nv.size(); j++)
                    nv[j].transfer_state();
            }
            ///*debug*/cout << endl;
            for (int i = 0; i < nv.size(); i++) nv[i].clear_flags(); //찾고 난 뒤 노드 플래그 초기화
            return matched(source, start, end, valid);
        }

        //주어진 문자열에서 매치되는 모든 매치 정보를 반환한다.
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
    cout << "\n문자열 \"" << test << "\"에는 \"" << regex << "\"패턴과 일치하는 부분이 ";
    cout << (!result.empty() ? "있습니다." : "없습니다.") << endl;
    if (result.empty()) return;

    for (const regex::matched& mt : result) {
        cout << "패턴과 일치하는 문자열은 \"" << mt.group() << "\"이고, "
            << "[" << mt.start() << "," << mt.end() << ") 구간에 속합니다.\n";
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
        cout << "\n\n\n<<< 테스트 #" << test << " >>>" << endl;
        regex::compiled cp = regex::compile(mock_regex[test], test);
        for (int i = 0; i < tests[test].size(); i++)
            print_result(cp.match_all(tests[test][i]), tests[test][i], mock_regex[test]);
    }

    return 0;
}



}

int main() { assignment1::main(); }
