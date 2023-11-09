//깃 컨벤션 참고 https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
using namespace std;

namespace assignment1 {

class MySimpleRegex {
//이 클래스는 static 메서드만 있으므로 생성자를 private로 둔다.
private: MySimpleRegex(); 
public:
    // 문자열에서 패턴과 일치된 범위를 표현한다.
    class matched {
    private:
        const int m_start, m_end;   //참조 문자열에서의 범위값
        const string& m_ref;        //참조 문자열
        const bool m_valid;         //객체가 유효한 지 여부

    public:
        matched(const string& ref, int start, int end, bool valid) : 
            m_ref(ref), m_start(start), m_end(end), m_valid(valid) {}

        //해당 객체가 유효한 지 여부 확인 (false는 유효하지 않음)
        bool valid() const { return m_valid; }

        //범위에 속하는 문자열 반환
        string group() const { return m_ref.substr(m_start, m_end - m_start); }

        //범위의 시작값
        int start() const { return m_start; }

        //범위의 끝값
        int end() const { return m_end; }

        //범위
        pair<int, int> span() const { return pair<int, int>(m_start, m_end); }
    };
    
    //TODO: 파싱 후 상태 기계 생성하는 알고리즘 구현해야 됨
    //정규표현식을 컴파일한 객체로, 확인할 문자열을 입력하면 일치 정보를 반환한다.
    class compiled {
        friend class MySimpleRegex;
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

        //노드를 카리키는 포인터 클래스(시그니처)
        class node_ptr;

        //유한 오토마타를 구성하는 노드
        class node {
        private:
            string m_name; // state node name
            int m_suspended_active_count; // 계류된 active_count
            bool m_suspended_state; // for syncronization: 계류된 state
            int m_active_count; // accept된 터미널 노드에서 인식된 문자열 길이를 의미한다.
            bool m_state;       // 0: deactive, 1: active
            bool m_isTerminal;  // 0: terminal, 1: non-terminal
            bool m_isAccepted;  // 0: non-accept,   1: accept

            Imatchable* matcher; // target character for matching
            vector<node_ptr*> next; // next link 

        public:
            //반복 범위값에 사용 가능하다. 무한대를 의미한다.
            static const unsigned INF = unsigned(-1); //2^31 - 1

            //생성자
            node(string name = "", bool isTerminal = false, Imatchable* match = nullptr) :
                m_name(name), m_isTerminal(isTerminal), matcher(match), next(0) {
                m_suspended_active_count = m_active_count = 0;
                m_suspended_state = m_state = false;
                m_isAccepted = false;
            }
          
            //소멸자: new로 할당받은 메모리를 반환한다.
            ~node() { 
                if (matcher != nullptr) 
                    delete matcher; 
                
                for (int i = 0; i < next.size(); i++)
                    if (next[i] != nullptr)
                        delete next[i]; 
            }

            //본 노드의 활성화 카운터 값을 반환한다.
            //accepted가 true일 때 이 값은 인식된 문자열의 길이를 의미한다.
            int active_count() { return m_active_count; }
            
            //본 노드가 가리키는 노드를 추가한다.
            void addNode(node_ptr* _next) { next.push_back(_next); }
            
            //for syncronization: 본 노드를 활성화하도록 요쳥한다. 상태값이 계류된다.
            //이후 transfer_state를 호출해 계류된 상태를 노드에 진짜 반영한다.
            void request_active(int next_active_count) {
                m_suspended_active_count = next_active_count;
                m_suspended_state = true; 
            }
            
            //노드 상태와 관련된 값과 플래그들을 모두 클리어한다.
            void clear_flags() {
                m_suspended_active_count = m_active_count = 0;
                m_suspended_state = m_state = false;
                m_isAccepted = false;
            }
           
            //본 노드에서 accept 되었는지 확인한다.
            bool is_accepted() {
                ///*debug*/ if (m_isAccepted) cout << "  >>>> accepted by " << m_name << endl;
                return m_isAccepted; //active 상태가 됐는데 터미널이면 accepted가 되었음(true 반환)
            }
            
            //계류된 상태들을 본 상태로 전송시킨다.
            void transfer_state() { //상태를 전이함
                m_active_count = m_suspended_active_count; //계류된 활성 카운터 전달
                m_state = m_suspended_state; //계류된 상태 전달

                m_suspended_active_count = 0;
                m_suspended_state = false;
            }

            //TODO: 정규식 맨 끝에 A*가 올 경우 생각해보기
            //노드에 한 문자를 입력함: 노드가 활성화됐을 때 문자가 매칭되면 다음 노드들에게 활성화를 요청함
            //노드 활성화 & 문자 매칭일 때 터미널 노드인 경우 is_accepted가 true가 됨
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
        
        //노드를 가리키는 포인터(가상 클래스)
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
       
        //TODO: 주어진 정규표현식으로 상태머신 생성하기 필요
        compiled(const string& regex = "", int test_case = 0) {

            //mock implementation
            //문법을 해석해 아래 작업이 알아서 되어야함
            switch (test_case) {
                // abc|ade
            case 0:
                n_vec.resize(5/*정규표현식 해석 후 노드 개수가 되어야 함*/);

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
        
        //소멸자 (메모리 반환 필요)
        ~compiled() {
            for (int i = 0; i < node_ptr_vector.size(); i++)
                if (node_ptr_vector[i] != nullptr)
                    delete node_ptr_vector[i];
        }

        //nodes
        vector<node *> n_vec;        
    public:

        //source에서 가장 처음으로 발견된 일치 정보를 반환한다.
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
                // nv[j]가 active 상태였다면 이 연산 후 nv[j]에서 나오는 화살표는
                // 조건을 만족할 시 가리키는 노드의 transited를 활성화한다.
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
                // transited가 활성화 된 모든 노드를 active로 변경한다.
                for (int j = 0; j < n_vec.size(); j++)
                    n_vec[j]->transfer_state();
            }
            ///*debug*/cout << endl;
            for (int i = 0; i < n_vec.size(); i++) n_vec[i]->clear_flags(); //찾고 난 뒤 노드 플래그 초기화
            return matched(source, start, end, valid);
        }

        //source에서 발견되는 모든 일치 정보를 반환한다.
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

    //정규표현식을 컴파일한 객체를 반환한다.
    static compiled compile(const string& regex, int test_case = 0) {
        return compiled(regex, test_case);
    }
    
    //source에서 정규표현식과 가장 먼저 일치하는 범위(유효하지 않을 수도 있음)를 구한다.
    static matched match(const string& regex, const string& source) {
        return compiled(regex).match(source);
    }
    
    //source에서 정규표현식과 일치하는 모든 범위를 구한다. 일치정보가 없을 경우 빈 vector가 반환된다.
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
        cout << "<테스트 #" << (t + 1) << " / 정규표현식: " << mock_regex[t] << ">" << endl;

        MySimpleRegex::compiled cp = MySimpleRegex::compile(mock_regex[t], t);
        for (int i = 0; i < tests[t].size(); i++) {
            vector<MySimpleRegex::matched> result = cp.match_all(tests[t][i]);
            cout << "\"" << tests[t][i] << "\"";
            if (result.empty()) {
                cout << " -> 일치구간 없음" << endl;
                continue;
            }
            cout << endl;

            //매치된 영역에 밑줄 표시
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
                cout << "    <구간 [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";
        }
        cout << "\n\n";
    }
}

int main() { run_test(); return 0; }

}

int main() { assignment1::main(); }
