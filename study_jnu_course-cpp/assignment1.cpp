//깃 컨벤션 참고 https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;


namespace assignment1 {

class MySimpleRegex {    
private: MySimpleRegex();   //이 클래스는 static 메서드만 있으므로 생성자를 private로 둔다.
public:
    class matched;  //문자열에서 패턴과 일치된 범위를 표현한다.
    class compiled; //정규표현식을 컴파일한 객체로, 확인할 문자열을 입력하면 일치 정보를 반환한다.
    
    //정규표현식을 컴파일한 객체를 반환한다.
    static compiled compile(const string& m_regex, int test_case);    
    //source에서 정규표현식과 가장 먼저 일치하는 범위(유효하지 않을 수도 있음)를 구한다.
    static matched match(const string& m_regex, const string& source);    
    //source에서 정규표현식과 일치하는 모든 범위를 구한다. 일치정보가 없을 경우 빈 vector가 반환된다.
    static vector<matched> match_all(const string m_regex, const string& source);
};

// 문자열에서 패턴과 일치된 범위를 표현한다.
class MySimpleRegex::matched {
private:
    const string& m_ref;            //참조 문자열
    const bool m_valid;             //객체 유효성
    const unsigned m_start, m_end;  //참조 문자열에서의 범위

public:
    matched(const string& ref, unsigned start, unsigned end, bool valid) :
        m_ref(ref), m_start(start), m_end(end), m_valid(valid) {}

    //해당 객체가 유효한 지 여부 확인 (false는 유효하지 않음)
    bool valid() const { return m_valid; }

    //범위에 속하는 문자열 반환
    string group() const { return m_ref.substr(m_start, m_end - m_start); }

    //범위의 시작값
    unsigned start() const { return m_start; }

    //범위의 끝값
    unsigned end() const { return m_end; }

    //범위
    pair<unsigned, unsigned> span() const { return pair<unsigned, unsigned>(m_start, m_end); }

    //유효하지 않은 matched 객체를 반환한다.
    static const matched invalid() { return matched("", 0, 0, false); }
};

//정규표현식을 컴파일한 객체로, 확인할 문자열을 입력하면 일치 정보를 반환한다.
class MySimpleRegex::compiled {
private:
    /****************************/
    /*       Inner Classes      */
    /****************************/

    /*매치 객체 인터페이스*/ class Imatchable {
    public:
        virtual bool test(char ch) = 0;
    };
    /*단일 문자 매치*/ class match_single : public Imatchable {
    private: char ch;
    public:
        match_single(char ch) : ch(ch) {}
        virtual bool test(char _ch) override { return ch == _ch; }
    };
    /*모든 문자 매치*/ class match_dot : public Imatchable {
    public:
        virtual bool test(char _ch) override {
            return ('A' <= _ch && _ch <= 'Z') || ('a' <= _ch && _ch <= 'z');
        }
    };

    /****************************/
    
    class node; //pre-defined class

    //액티브를 요청할 때 전달하는 정보
    struct active_request_info {
        node* target;
        unsigned start_index;
    };

    //노드를 가리키는 포인터(가상 클래스)
    class node_ptr {
    protected: 
        node* m_pNode;
        Imatchable* matcher;

        //각 포인터가 수행할 전이 동작을 수행한다.
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) = 0;

    public:
        static const unsigned INF = ~0U; //2^32 - 1
        
        node_ptr(Imatchable* match, node* pNode) : m_pNode(pNode), matcher(match) {}
        ~node_ptr() { if (matcher != nullptr) delete matcher; }

        //포인터와 관련된 플래그를 클리어한다. 필요한 경우만 오버라이드한다.
        virtual void clear_flag() {}
        
        //연결된 노드에 active를 요청한다. 한번에 될 수도 있고, 그렇지 않을 수도 있음
        virtual void active_request(vector<active_request_info>& next_active, unsigned state_istart, char ch) final {
            //요구되는 문자와 일치되는 경우에만 액션을 취함
            if (matcher->test(ch)) transition_action(next_active, state_istart);
        }
        
        //연결된 노드의 역참조를 주어진 노드로 연결한다
        virtual void link_reverse_ref(node* ref) final {
            m_pNode->add_link_reverse_ref(ref);
        }      
    };

    //노드를 직접 연결하는 포인터
    //전이 요청을 받으면 무조건 전이 신호 보내줌
    class node_ptr_direct : public node_ptr {
    protected:
        virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) override {
            next_active.push_back({ m_pNode, state_istart });
        }

    public:
        node_ptr_direct(Imatchable* matcher, node* pNode) : node_ptr(matcher, pNode) {}

    };

    //노드를 조건부로 연결하는 포인터
    //전이 요청을 일정 횟수 받은 경우에만 전이 신호 보내줌
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

    //유한 오토마타를 구성하는 노드
    class node {
    private:
        string m_name; // state node name
        unsigned m_state_istart;    // state가 첫 active되었을 때 입력되었던 문자의 인덱스
        bool m_state_active;        // 0: deactive, 1: active

        bool m_is_terminal;  // 0: terminal, 1: non-terminal

        vector<node_ptr*> next; // next link
        vector<node*> reverse_ref; //이 노드를 가리키는 역참조

    public:
        //반복 범위값에 사용 가능하다. 무한대를 의미한다.
        //static const unsigned INF = unsigned(-1); //2^31 - 1

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

        //처음으로 활성화된 상태였을 때 첫 인덱스를 반환한다.
        //is_accepted일 때 인식된 문자열의 첫 위치를 의미한다.
        unsigned index_start() const { return m_state_istart; }

        //본 노드가 가리키는 노드를 추가한다.
        void addNode(node_ptr* _next) { 
            next.push_back(_next); 
            _next->link_reverse_ref(this);
        }
    
        //역링크를 추가한다.
        void add_link_reverse_ref(node* ref) {
            reverse_ref.push_back(ref);
        }

        //노드를 활성화 상태로 만든다.
        void active(int state_istart) {
            m_state_istart = state_istart;
            m_state_active = true;
        }

        //노드 상태와 관련된 값과 플래그들을 모두 클리어한다.
        void clear_flags() {
            m_state_istart = 0;
            m_state_active = false;

            for (node_ptr* p : next)
                p->clear_flag();
        }

        //본 노드에서 accept 되었는지 확인한다.
        bool is_accepted() const {
            return m_is_terminal && m_state_active;
        }

        //노드에 한 문자를 입력함: 노드가 활성화됐을 때 문자가 매칭되면 다음 노드들에게 활성화를 요청함
        //노드 활성화 & 문자 매칭일 때 터미널 노드인 경우 is_accepted가 true가 됨
        void input(vector<active_request_info>& next_active, const char ch) {
            if (!m_state_active) return;

            for (int i = 0; i < next.size(); i++) {
                next[i]->active_request(next_active, m_state_istart, ch);
            }

            //인식되었을 경우 true
            //m_isAccepted = m_state_active && match && m_isTerminal;
            m_state_active = false; // deactivate this node after matching
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

    //TODO: 주어진 정규표현식으로 상태머신 생성하기 필요
    //상태기계 노드 컨테이너에 상태기계를 생성한다.
    void create_state_machine(int test_case) {

        //mock implementation
        //문법을 해석해 아래 작업이 알아서 되어야함
        if (test_case == 1) {
            // abc|ade
            m_node.resize(6/*정규표현식 해석 후 노드 개수가 되어야 함*/);

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

            // 엡실론 신호 받는 노드 설정
            m_get_epsilon.push_back(m_node[0]);

            // 터미널 노드 설정
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
            //새로운 노드포인터!!
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

    //생성된 상태기계를 삭제한다.
    void delete_state_machine() {
        for (int i = 0; i < m_node.size(); i++)
            if (m_node[i] != nullptr)
                delete m_node[i];
    }

    //TODO: T* , T+ 같은 케이스에서 그리디하게 동작하게 바꿔야 함
    //상태기계에 문자열을 입력으로 받고, 가장 처음으로 accept된 일치 정보를 출력한다.
    matched state_machine_input(const string& src, unsigned index_start = 0, bool check_at_front_only = false) {
        map<int, matched*> found;
        vector<node*> actives;
        vector<active_request_info> next_actives;

        ///*debug*/cout << "test for \"" << source << "\"" << endl;
        for (unsigned i = index_start; i < src.length(); i++) {
            ///*debug*/cout << "  >> input " << src[i] << endl;
            //엡실론 신호를 매 주기기마다 부여함
            //check_at_front_only면 문자열 시작이 패턴과 일치하는 경우만 확인한다.
            //즉 최초 1회만 엡실론 신호를 부여한다.
            if (!check_at_front_only || i == 0) {
                for (node*& target : m_get_epsilon) {
                    target->active(i);
                    actives.push_back(target);
                }
            }
                
            // give ch all nodes
            // nv[j]가 active 상태였다면 이 연산 후 nv[j]에서 나오는 화살표는
            // 조건을 만족할 시 가리키는 노드의 transited를 활성화한다.
            while (!actives.empty()) {
                node* target = actives.back();
                target->input(next_actives, src[i]);
                actives.pop_back();
            }

            //다음 활성화될 노드 리스트를 받아오고 활성화한다.
            while (!next_actives.empty()) {
                active_request_info& info = next_actives.back();
                (info.target)->active(info.start_index);
                actives.push_back(info.target);
                next_actives.pop_back();
            }

            //터미널 노드 순회 후 accepted되었으면 matched를 생성한다.
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
        //src를 모두 input한 뒤 모든 노드의 플래그 초기화
        for (node* target : m_node) target->clear_flags();


        if (found.empty()) //일치하는 패턴 없음
            return matched::invalid();
        else {
            matched ret = *found.begin()->second;
            for (auto& p : found)
                delete p.second;

            return ret;
        }
    }



public:
    //주어진 정규표현식으로 내부적으로 상태기계를 생성한다.
    compiled(const string& m_regex = "", int test_case = 0) : m_regex(m_regex) {
        create_state_machine(test_case);
    }

    //소멸자 (메모리 반환 필요)
    ~compiled() { delete_state_machine(); }

    //source에서 start_idx부터 탐색을 시작해 가장 처음으로 발견된 일치 정보를 반환한다.
    matched match(const string& source, unsigned index_start = 0) {
        return state_machine_input(source, index_start, false);
    }

    //source에서 발견되는 모든 일치 정보를 반환한다.
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
    struct test_case {
        int number;
        string regex;
        vector<string> test;
        vector<matches> expect;
        bool details;
        bool enabled;
    };
    vector<test_case> test_cases;

    void run_tests() {
        set<int> disabled;
        for (int t = 1; t < test_cases.size(); t++) 
            if (!test_cases[t].enabled) disabled.insert(test_cases[t].number);

        if (!disabled.empty()) {
            cout << "[스킵된 테스트 목록]  ";
            for (int skip: disabled) cout << skip << ";  ";
            cout << "\n\n\n";
        }

        for (int test = 1; test < test_cases.size(); test++) {
            if (disabled.find(test) != disabled.end()) continue; //테스트 스킵

            bool test_successed = true;
            bool regex_printed = false;
            string indent = "   ";
            vector<bool> results = run_test(test_cases[test].number); //test 실행
            for (int i = 0; i < results.size(); i++) {
                if (!results[i]) {
                    test_successed = false;
                    //실패한 실행 결과 생성
                    MySimpleRegex::compiled cp = MySimpleRegex::compile(test_cases[test].regex, test);
                    matches result = cp.match_all(test_cases[test].test[i]);

                    //실패한 테스트 제목 출력
                    if (!regex_printed) {
                        regex_printed = true;
                        cout << "\n\n\n";
                        cout << "<<<< 테스트 #" << test << " 실패 >>>>" << endl;
                        cout << "정규표현식: \"" << test_cases[test].regex << "\"\n";
                    }

                    //실패한 케이스 출력
                    cout << "케이스 #" << (i + 1) << endl;
                    cout << indent << "< 기대 결과 >" << endl;
                    cout << indent << "\"" << test_cases[test].test[i] << "\"\n";
                    unit_tester::match_info_print(test_cases[test].expect[i], indent + " ");
                    for (const MySimpleRegex::matched& mt : test_cases[test].expect[i])
                        cout << "\t<구간 [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";

                    cout << indent << "< 실행 결과 >" << endl;
                    cout << indent << "\"" << test_cases[test].test[i] << "\"\n";
                    unit_tester::match_info_print(result, indent + " ");
                    for (const MySimpleRegex::matched& mt : result)
                        cout << "\t<구간 [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";


                }
                if (!test_successed && i + 1 == results.size()) cout << "\n\n";
            }

            if (test_successed) {
                if (test_cases[test].details) {
                    cout << "\n\n\n";
                    cout << "<테스트 #" << test << " 통과 (detailed mode enabled)>" << endl;
                    cout << "정규표현식: \"" << test_cases[test].regex << "\"\n";

                    MySimpleRegex::compiled cp = MySimpleRegex::compile(test_cases[test].regex, test);
                    for (int i = 0; i < test_cases[test].test.size(); i++) {
                        vector<MySimpleRegex::matched> result = cp.match_all(test_cases[test].test[i]);
                        cout << indent << "\"" << test_cases[test].test[i] << "\"";
                        if (result.empty()) {
                            cout << " -> 일치구간 없음" << endl;
                            continue;
                        }
                        cout << endl;

                        cout << indent << "< 실행 결과 >" << endl;
                        cout << indent << "\"" << test_cases[test].test[i] << "\"\n";
                        unit_tester::match_info_print(result, indent + " ");
                        for (const MySimpleRegex::matched& mt : result)
                            cout << "\t<구간 [" << mt.start() << "," << mt.end() << ")> : " << "\"" << mt.group() << "\"\n";
                    }
                    cout << "\n\n";
                }
                else cout << "테스트 #" << test << " 통과>" << endl;
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

    vector<bool> run_test(int t) {
        vector<bool> ret;

        MySimpleRegex::compiled cp = MySimpleRegex::compile(test_cases[t].regex, t);
        for (int i = 0; i < test_cases[t].test.size(); i++) {
            matches list_all = cp.match_all(test_cases[t].test[i]);
            ret.push_back(assertEqual(test_cases[t].expect[i], list_all));
        }

        return ret;
    }

    //매치된 영역에 밑줄 표시
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

public:
    unit_tester() { 
        register_tests();
        enable_all().details_hide_all();
    }

    void run() { run_tests(); }

    unit_tester& details_hide(int test_number) {
        test_cases[test_number].details = false;
        return *this;
    }
    unit_tester& details_show(int test_number) {
        test_cases[test_number].details = true; 
        return *this;
    }
    unit_tester& details_hide_all() {
        for (test_case& t : test_cases) t.details = false;
        return *this;
    }
    unit_tester& details_show_all() {
        for (test_case& t : test_cases) t.details = true;
        return *this;
    }

    unit_tester& enable(int test_number) {
        test_cases[test_number].enabled = true;
        return *this;
    }
    unit_tester& disable(int test_number) {
        test_cases[test_number].enabled = false;
        return *this;
    }
    unit_tester& enable_all() {
        for (test_case& t : test_cases) t.enabled = true;
        return *this;
    }
    unit_tester& disable_all() {
        for (test_case& t : test_cases) t.enabled = false;
        return *this;
    }

protected:

    //실행할 테스트 등록
    void register_tests() {
        test0(); //register example (<!> do not delete <!>)
        test1(); test2(); test3(); test4(); test5();
        test6(); test7(); test8(); test9();
    }

    //test func example
    void test0() {
        int t = 0;
        vector<string> tests({
            "test_str",
            "test_str"});

        test_cases.push_back({
            t, "test_regex", tests,
        {{
            match(tests[0], 0, 0, true)},
        {
            match(tests[1], 0, 0, true)}}});
    }
    
    void test1() {
        int t = 1;
        static vector<string> tests({
            "abc",
            "ade",
            "aaaaabbbbabcaaaaaadeaaaaabc",
            "abbbbbbbbdddddddfdfdfdfdf",
            "zzosiduabcfdsfdafdadere",
            "aosisudababaifudiaddadeidufodi",
            "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" });
        
        test_cases.push_back({
            t,  "abc|ade", tests,
        {{
            match(tests[0], 0, 3, true)},
        {
            match(tests[1], 0, 3, true) },
        {
            match(tests[2], 9, 12, true),
            match(tests[2], 17, 20, true),
            match(tests[2], 24, 27, true) },
        { },
        {
            match(tests[4], 7, 10, true),
            match(tests[4], 18, 21, true), },
        {
            match(tests[5], 20, 23, true) },
        {
            match(tests[6], 23, 26, true),
            match(tests[6], 27, 30, true),
            match(tests[6], 100, 103, true),
            match(tests[6], 107, 110, true)}}});    
    }
    void test2() {
        int t = 2;
        static vector<string> tests({
            "aba",
            "bab",
            "baba",
            "ababababababababababab" });

        test_cases.push_back({
            t, "aba", tests,
        {{
            match(tests[0], 0, 3, true)},
        { },
        {
            match(tests[2], 1, 4, true)},
        {
            match(tests[3], 0, 3, true),
            match(tests[3], 4, 7, true),
            match(tests[3], 8, 11, true),
            match(tests[3], 12, 15, true),
            match(tests[3], 16, 19, true)}}});
    }
    void test3() {
        int t = 3;
        static vector<string> tests({
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

        test_cases.push_back({
            t, "NK((abc|ABC)*N|(OP)+)Q", tests,
        {{
            match(tests[0], 0, 4, true)},
        {
            match(tests[1], 0, 5, true)},
        { },
        { },
        {
            match(tests[4], 0, 13, true)},
        { },
        {
            match(tests[6], 0, 10, true)},
        {
            match(tests[7], 0, 25, true)},
        {
            match(tests[8], 0, 11, true)},
        {
            match(tests[9], 5, 9, true),
            match(tests[9], 20, 27, true),
            match(tests[9], 29, 36, true),
            match(tests[9], 41, 48, true)}}});
    }
    void test4() {
        int t = 4;
        static vector<string> tests({
            "a",
            "ac",
            "abc",
            "abbc",
            "abbbbbb",
            "abbbc",
            "abbbbbc",
            "abbbbababcccccabbbbcabbbbabacacabccccab" });

        test_cases.push_back({
            t, "ab+c", tests,
        {{ },
        { },
        {
            match(tests[2], 0, 3, true)},
        {
            match(tests[3], 0, 4, true)},
        { },
        {
            match(tests[5], 0, 5, true)},
        {
            match(tests[6], 0, 7, true)},
        {
            match(tests[7], 7, 10, true),
            match(tests[7], 14, 20, true),
            match(tests[7], 31, 34, true)}}});
    }
    void test5() {
        int t = 5;
        static vector<string> tests({
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

        test_cases.push_back({
            t, "abc+", tests,
        {{ }, { }, { }, { }, { },
        {
            match(tests[5], 0, 3, true)},
        {
            match(tests[6], 0, 4, true)},
        {
            match(tests[7], 0, 5, true)},
        {
            match(tests[8], 0, 8, true)},
        {
            match(tests[9], 3, 13, true),
            match(tests[9], 14, 22, true),
            match(tests[9], 22, 29, true),
            match(tests[9], 37, 50, true)}}});
    }
    void test6() {
        int t = 6;
        static vector<string> tests({
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

        test_cases.push_back({ 
            t, "aB{3,6}c", tests,
        {{ }, { }, { },
        {
            match(tests[3], 0, 5, true)},
        {
            match(tests[4], 0, 6, true)},
        {
            match(tests[5], 0, 7, true)},
        {
            match(tests[6], 1, 9, true)},
        { }, { }, { }, { }, { }, { }, { },
        {
            match(tests[14], 1, 6, true),
            match(tests[14], 7, 13, true),
            match(tests[14], 20, 28, true),
            match(tests[14], 31, 37, true),
            match(tests[14], 47, 54, true)}} });
    }
    void test7() {
        int t = 7;
        static vector<string> tests({
            "jkTT",
            "jkTTT",
            "jkTTTT",
            "jkTTTTT",
            "jkTTTTTT",
            "jkTTTTTTT" });

        test_cases.push_back({
            t, "jkT{4,5}", tests,
        {{ }, { },
        {
            match(tests[2], 0, 6, true)},
        {
            match(tests[3], 0, 7, true)},
        {
            match(tests[4], 0, 7, true)},
        {
            match(tests[5], 0, 7, true)}}});
    }
    void test8() {
        int t = 8;
        static vector<string> tests({
            "TT",
            "TTT",
            "TTTT",
            "TTTTT",
            "TTTTTT",
            "TTTTTTT" });

        test_cases.push_back({
            t, "T{3,5}", tests,
        {{ },
        {
            match(tests[1], 0, 3, true)},
        {
            match(tests[2], 0, 4, true)},
        {
            match(tests[3], 0, 5, true)},
        { }, { }    }});
    }
    void test9() {
        int t = 9;
        static vector<string> tests({
            "k",
            "T",
            "TT",
            "TTT",
            "kkTTTT",
            "TTTTT",
            "kkkkTTTTTT",
            "TTTTTTT",
            "kkkkTTTTTTz" });

        test_cases.push_back({
            t, "T+", tests,
        {
        { },
        {
            match(tests[1], 0, 1, true)},
        {
            match(tests[2], 0, 2, true)},
        {
            match(tests[3], 0, 3, true)},
        {
            match(tests[4], 2, 6, true)},
        {
            match(tests[5], 0, 5, true)},
        {
            match(tests[6], 4, 10, true)},
        {
            match(tests[7], 0, 7, true)},
        {
            match(tests[8], 4, 10, true)}}});;
    }
    
};

int main() {
    unit_tester test;
    test.disable_all(); 
    test.enable(1).enable(2).enable(5).enable(7).run();
    return 0;
}

}

int main() { assignment1::main(); }
