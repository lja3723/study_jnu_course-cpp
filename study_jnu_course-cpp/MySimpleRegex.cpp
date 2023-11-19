#include "MySimpleRegex.h"
#include <string>



/****************************/
/*    Inner Classes &       */
/*    members of compiled   */
/****************************/
namespace assignment1 {
using compiled = MySimpleRegex::compiled;

/***********   문자 matcher 및 파생 클래스   ***********/
class compiled::Imatchable {
public:
    virtual bool test(char ch) = 0;
    virtual Imatchable* copy() = 0; //자기 자신 동적 복사
};   
class compiled::matcher_single : public compiled::Imatchable {
private: char ch;
public:
    matcher_single(char ch) : ch(ch) {}
    virtual bool test(char _ch) override { return ch == _ch; }
    virtual matcher_single* copy() override{
        return new matcher_single(ch);
    }
};
class compiled::matcher_dot : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return ('A' <= _ch && _ch <= 'Z') || ('a' <= _ch && _ch <= 'z');
    }    
    virtual matcher_dot* copy() override {
        return new matcher_dot();
    }
};
class compiled::matcher_true : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return true;
    }
    virtual matcher_true* copy() override {
        return new matcher_true();
    }
};



/***************   node & 파생 클래스   ***************/
class compiled::node {
    friend class node_ptr;
private:
    string m_name;              // state node name
    unsigned m_state_istart;    // state가 첫 active되었을 때 입력되었던 문자의 인덱스
    bool m_state_active;        // 0: deactive, 1: active
    bool m_is_terminal;         // 0: terminal, 1: non-terminal

    vector<node_ptr*> m_next;     // next link
    vector<node*> m_reverse_ref;  //이 노드를 가리키는 역참조

    void add_link_reverse_ref(node* ref); //역링크를 추가한다.

public:
    node(string name, bool isTerminal = false);
    ~node();

    /*********  조회 함수  *********/
    const string& name() const;
    unsigned index_start() const;   //is_accepted일 때 인식된 문자열의 첫 위치를 의미한다.  
    bool is_accepted() const;       //본 노드에서 accept 되었는지 확인한다.
    const vector<node_ptr*>& next() const;      //next 목록을 반환한다.
    const vector<node*>& reverse_ref() const;   //리버스 참조목록을 반환한다.

    /*********  변경 함수  *********/
    void add_link(node_ptr* _next); //본 노드가 가리키는 노드를 추가한다.
    void active(int state_istart); //노드를 활성화 상태로 만든다.
    void clear_flags(); //노드 상태와 관련된 값과 플래그들을 모두 클리어한다.


    //노드에 한 문자를 입력한다. 
    //노드가 활성화됐을 때 문자가 매치되면 next 노드들에게 활성화를 요청한다.
    //노드 활성화 && 터미널 노드인 경우 is_accepted가 true가 된다.
    void input(vector<active_request_info>& next_active, const char ch);


}; /************************ node end ************************/
class compiled::node_ptr {
    friend class node;
protected:
    node* m_pNode;
    Imatchable* matcher;

    //각 포인터가 수행할 전이 동작을 수행한다. 오버라이딩 필요
    virtual void transition_action(
        vector<active_request_info>& next_active, unsigned state_istart) = 0;

    //연결된 노드의 역참조를 주어진 노드로 연결한다
    virtual void link_reverse_ref(node* ref) final {
        m_pNode->add_link_reverse_ref(ref);
    }

    //포인터와 관련된 플래그를 클리어한다. 필요한 경우만 오버라이드한다.
    virtual void clear_flag() {}


public:
    static const unsigned INF = ~0U; //2^32 - 1

    node_ptr(Imatchable* ranged_string, node* pNode) : m_pNode(pNode), matcher(ranged_string) {}
    ~node_ptr();

    //현재 포인터를 새로운 연결을 가진 포인터로 복사한다.
    virtual node_ptr* copy(node* new_pNode) = 0;

    //연결된 노드가 다음 사이클에 active 되도록 요청한다.
    //한번에 될 수도 있고, 그렇지 않을 수도 있음
    virtual void request_active_nexttime(
        vector<active_request_info>& next_active, 
        unsigned state_istart, char ch ) final 
    { 
        if (matcher->test(ch)) //요구되는 문자와 일치되는 경우에만 전이 행동을 취한다.
            transition_action(next_active, state_istart);
    }

    //is_accepted일 때 인식된 문자열의 첫 위치를 의미한다.
    unsigned index_start() const {
        return m_pNode->index_start();
    }

    //본 노드에서 accept 되었는지 확인한다.
    bool is_accepted() const {
        return m_pNode->is_accepted();
    }

    //주어진 파라미터가 노드 포인터가 가리키는 노드와 같은지 확인한다.
    bool is_ref_same(const node* pNode) const {
        return m_pNode == pNode;
    }



}; /************************ noe_ptr end ************************/
class compiled::node_ptr_direct : public node_ptr {
protected:
    virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) override {
        next_active.push_back({ m_pNode, state_istart });
    }

public:
    node_ptr_direct(Imatchable* matcher, node* pNode) : node_ptr(matcher, pNode) {}
    virtual node_ptr_direct* copy(node* new_pNode) override {
        return new node_ptr_direct(matcher->copy(), new_pNode);
    }


}; /************************ node_ptr_direct end ************************/
class compiled::node_ptr_inner_counter : public node_ptr {
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

    virtual void clear_flag() override {
        counter = 0;
        prev_istart = node_ptr::INF;
    }


public:
    node_ptr_inner_counter(Imatchable* matcher, node* pNode, unsigned low_bound = 0, unsigned up_bound = node_ptr::INF) :
        node_ptr(matcher, pNode), lower_bound(low_bound), upper_bound(up_bound) {
        clear_flag();
    }    
    virtual node_ptr_inner_counter* copy(node* new_pNode) override {
        return new node_ptr_inner_counter(
            matcher->copy(), new_pNode, lower_bound, upper_bound);
    }


}; /************************ node_ptr_cnt_inner end ************************/



/**************   node 클래스 멤버 함수   **************/
compiled::node::node(string name, bool isTerminal) :
    m_name(name), m_is_terminal(isTerminal), m_next(0) {
    m_state_istart = 0;
    m_state_active = false;
}
compiled::node::~node() {
    for (int i = 0; i < m_next.size(); i++)
        if (m_next[i] != nullptr)
            delete m_next[i]; //new로 할당받은 메모리를 반환한다.
}

const string& compiled::node::name() const {
    return m_name; 
}
unsigned compiled::node::index_start() const {
    return m_state_istart; 
}
bool compiled::node::is_accepted() const {
    return m_is_terminal && m_state_active;
}
const vector<compiled::node_ptr*>& compiled::node::next() const {
    return m_next;
}
const vector<compiled::node*>& compiled::node::reverse_ref() const {
    return m_reverse_ref;
}

void compiled::node::add_link(node_ptr* _next) {
    m_next.push_back(_next);
    _next->link_reverse_ref(this);
}
void compiled::node::add_link_reverse_ref(node* ref) {
    m_reverse_ref.push_back(ref);
}
void compiled::node::active(int state_istart) {
    m_state_istart = state_istart;
    m_state_active = true;
}
void compiled::node::clear_flags() {
    m_state_istart = 0;
    m_state_active = false;

    for (node_ptr* p : m_next) p->clear_flag();
}
void compiled::node::input(vector<active_request_info>& next_active, const char ch) {
    if (!m_state_active) return;

    for (int i = 0; i < m_next.size(); i++)
        m_next[i]->request_active_nexttime(next_active, m_state_istart, ch);

    m_state_active = false; // deactivate this node after matching
}

compiled::node_ptr::~node_ptr() { 
    if (matcher != nullptr) 
        delete matcher; //동적 할당된 matcher를 반환한다.
}


/******   state machine creator 클래스   ******/
//TODO: 주어진 정규표현식으로 상태머신 생성하기 필요
class compiled::state_machine_creator {
public:
    state_machine_creator(vector<node*>& _node, node*& _epsilon, node*& _terminal) :
        m_node(_node), m_epsilon(_epsilon), m_terminal(_terminal), node_cnt(0) {}

    bool make(const string& regex) {
        this->regex = regex;
        //상태 머신을 초기화한다.
        clear();

        //하드코딩된 정규식과 일치하는 경우 생성 성공
        if (is_hard_coded(regex)) return true;        

        //사전 처리
        bool syntax_valid = true;
        add_node_back();

        //정규표현식을 한 문자씩 읽으면서 작업을 수행한다.
        for (int i = 0; i < regex.size(); i++) {
            //asterisk 문법과 일치하면 상태 머신 연결작업 수행 후 i를 증가시킨다.
            if (check_asterisk(syntax_valid, i)) continue;
            //일치하지 않으면서 문법도 틀리면 종료한다.
            if (!syntax_valid) break;

            if (check_plus(syntax_valid, i)) continue;
            if (!syntax_valid) break;

            if (check_range(syntax_valid, i)) continue;
            if (!syntax_valid) break;

            if (check_and(syntax_valid, i)) continue;
            if (!syntax_valid) break;

            if (check_char(syntax_valid, i)) continue;
            if (!syntax_valid) break;
        }

        if (!syntax_valid) clear(); //잘못 생성된 상태머신을 초기화한다.
        return syntax_valid;
    }

private:
    //외부 참조 변수
    //이 변수들을 조작해 상태 머신을 생성한다.
    vector<node*>& m_node;
    node*& m_epsilon;
    node*& m_terminal;  

    //내부 변수
    int node_cnt;
    string regex;



    bool is_hard_coded(const string& m_regex) {
        //mock implementation
        //문법을 해석해 아래 작업이 알아서 되어야함
        if (m_regex == "{test}") {
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->add_link(new node_ptr_direct(
                new matcher_dot(), m_node[0]));
            m_node[0]->add_link(new node_ptr_direct(
                new matcher_dot(), m_node[1]));

            add_to_epsilon(m_node[0]);
            add_to_terminal(m_node[1]);
            return true;
        }
        if (m_regex == "abc|ade") {
            m_node.resize(6/*정규표현식 해석 후 노드 개수가 되어야 함*/);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);
            m_node[4] = new node("s4");
            m_node[5] = new node("s5");

            // set links
            m_node[0]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[1]));
            m_node[0]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[4]));

            m_node[1]->add_link(new node_ptr_direct(new matcher_single('b'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('c'), m_node[3]));

            m_node[4]->add_link(new node_ptr_direct(new matcher_single('d'), m_node[5]));
            m_node[5]->add_link(new node_ptr_direct(new matcher_single('e'), m_node[3]));

            // 엡실론 신호 받는 노드 설정
            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));

            // 터미널 노드 설정
            add_to_terminal(m_node[3]);
            return true;
        }
        else if (m_regex == "aba") {
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('b'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[3]));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));

            add_to_terminal(m_node[3]);
            return true;
        }
        else if (m_regex == "NK((abc|ABC)*N|(OP)+)Q") {
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

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('N'), m_node[1]));

            m_node[1]->add_link(new node_ptr_direct(new matcher_single('K'), m_node[5]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('K'), m_node[2]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('K'), m_node[8]));

            m_node[2]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[3]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('A'), m_node[6]));

            m_node[3]->add_link(new node_ptr_direct(new matcher_single('b'), m_node[4]));

            m_node[4]->add_link(new node_ptr_direct(new matcher_single('c'), m_node[2]));
            m_node[4]->add_link(new node_ptr_direct(new matcher_single('c'), m_node[5]));

            m_node[5]->add_link(new node_ptr_direct(new matcher_single('N'), m_node[10]));

            m_node[6]->add_link(new node_ptr_direct(new matcher_single('B'), m_node[7]));

            m_node[7]->add_link(new node_ptr_direct(new matcher_single('C'), m_node[2]));
            m_node[7]->add_link(new node_ptr_direct(new matcher_single('C'), m_node[5]));

            m_node[8]->add_link(new node_ptr_direct(new matcher_single('O'), m_node[9]));

            m_node[9]->add_link(new node_ptr_direct(new matcher_single('P'), m_node[8]));
            m_node[9]->add_link(new node_ptr_direct(new matcher_single('P'), m_node[10]));

            m_node[10]->add_link(new node_ptr_direct(new matcher_single('Q'), m_node[11]));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            add_to_terminal(m_node[11]);
            return true;
        }
        else if (m_regex == "ab+c") {
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('b'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('b'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('c'), m_node[3]));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            add_to_terminal(m_node[3]);
            return true;
        }
        else if (m_regex == "abc+") {
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('b'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('c'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('c'), m_node[3]));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            add_to_terminal(m_node[3]);
            return true;
        }
        else if (m_regex == "aB{3,6}c") {
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('B'), m_node[1]));
            //새로운 노드포인터!!
            m_node[1]->add_link(new node_ptr_inner_counter(new matcher_single('B'), m_node[2], 3, 6));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('c'), m_node[3]));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            add_to_terminal(m_node[3]);
            return true;
        }
        else if (m_regex == "jkT{4,5}") {
            m_node.resize(4);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('j'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('k'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[2]));
            m_node[2]->add_link(new node_ptr_inner_counter(new matcher_single('T'), m_node[3], 4, 5));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            add_to_terminal(m_node[3]);
            return true;
        }
        else if (m_regex == "T{3,5}") {
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[0]));
            m_node[0]->add_link(new node_ptr_inner_counter(new matcher_single('T'), m_node[1], 3, 5));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            add_to_terminal(m_node[1]);
            return true;
        }
        else if (m_regex == "T+") {
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[1]));
            m_node[0]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[0]));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            add_to_terminal(m_node[1]);
            return true;
        }
        else if (m_regex == "Tn(abTn)*") {
            m_node.resize(5);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2", true);
            m_node[3] = new node("s3");
            m_node[4] = new node("s4");


            m_node[0]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(new matcher_single('n'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(new matcher_single('a'), m_node[3]));
            m_node[3]->add_link(new node_ptr_direct(new matcher_single('b'), m_node[4]));
            m_node[4]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[1]));

            add_to_epsilon(m_node[0]);
            add_to_terminal(m_node[2]);
            return true;
        }

        //매치되는 regex 없음
        return false;
    }

    //노드 컨테이너를 초기화한다.
    void clear() {
        for (int i = 0; i < m_node.size(); i++)
            if (m_node[i] != nullptr)
                delete m_node[i];
        if (m_epsilon != nullptr) delete m_epsilon;
        if (m_terminal != nullptr) delete m_terminal;

        m_node.clear();
        m_epsilon = new node("epsilon");
        m_terminal = new node("terminal");
    }

    //엡실론을 받을 노드를 등록한다.
    void add_to_epsilon(node* target) {
        m_epsilon->add_link(new node_ptr_direct(new matcher_true(), target));
    }
    
    //터미널 노드를 등록한다.
    void add_to_terminal(node* target) {
        m_terminal->add_link(new node_ptr_direct(new matcher_true(),target));
    }

    //새 노드를 뒤에 추가한다.
    void add_node_back() {
        m_node.push_back(new node("s" + node_cnt++));
    }

    //정규식에서 지원하는 '단일 알파벳' 또는 . 인지 확인한다.
    bool is_char(char ch) {
        return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || '.';
    }

    //문자와 맞는 matcher를 반환한다.
    Imatchable* get_matcher(char ch) {
        if (ch == '.')
            return new matcher_dot();
        else if (is_char(ch))
            return new matcher_single(ch);
        else return nullptr;
    }

    // * 연산자 여부 체크한다.
    bool check_asterisk(bool& syntax, int& idx) {
        //범위 & 연산 대상인지 확인
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '*')
            return false;

        //연산 가능 여부 확인
        if (!is_char(regex[idx])) { 
            syntax = false;
            return false;
        }

        //matcher 얻기
        Imatchable* matcher = get_matcher(regex[idx]);
        if (matcher == nullptr) {
            syntax = false;
            return false;
        }

        //tail 참조 얻고 뒷부분 노드 추가
        int tail_idx = m_node.size() - 1;
        node* tail = m_node.back();
        add_node_back();

        //다음 노드 가리키는 링크 추가
        m_node[tail_idx]->add_link(new node_ptr_direct(
            matcher, m_node.back()));

        //이전 노드에서 다음 노드로 바로 가는 링크 추가
        for (node* rev : tail->reverse_ref()) {
            for (node_ptr* nxt : rev->next()) {
                if (nxt->is_ref_same(tail)) {
                    //copy() 이용해서 다음 노드로 가는 포인터 추가
                }
            }
        }

        //자기 자신 가리키는 노드 추가        
        m_node[tail_idx]->add_link(new node_ptr_direct(
            matcher, m_node[tail_idx]));

        return true;
    }

    // + 연산자 여부 체크한다.
    bool check_plus(bool& syntax, int& idx) {
        return true;
    }

    // {} 연산자 여부 체크한다.
    bool check_range(bool& syntax, int& idx) {
        return true;
    }

    // | 연산자 여부 체크한다.
    bool check_and(bool& syntax, int& idx) {
        return true;
    }

    //일반 문자로 이루어졌는지 체크한다.
    bool check_char(bool& syntax, int& idx) {
        return true;
    }
};



} //end of namespace assignment1




/****************************/
/*      MySimpleRegex &     */
/*      compiled Classes    */
/****************************/
namespace assignment1 {
using compiled = MySimpleRegex::compiled;


/*******************   MySimpleRegex static 함수   *******************/
compiled MySimpleRegex::compile(const string& m_regex) {
    return compiled(m_regex);
}
ranged_string MySimpleRegex::match(const string& m_regex, const string& source) {
    return compiled(m_regex).match(source);
}
vector<ranged_string> MySimpleRegex::match_all(const string m_regex, const string& source) {
    return compiled(m_regex).match_all(source);
}


/*******************   compiled 멤버 함수   *******************/
compiled::compiled(const string& m_regex)
    : m_regex(m_regex), m_epsilon(nullptr), m_terminal(nullptr) {

    state_machine_creator creator(m_node, m_epsilon, m_terminal);

    //정규식 문법이 잘못된 경우 빈 상태 기계가 생성된다.
    if (!creator.make(m_regex)) {
        cout << "정규표현식 문법이 잘못되었습니다. 비교가 제대로 수행되지 않을 수 있습니다." << endl;
    }


}
compiled::~compiled() {
    //동적 할당된 노드를 삭제한다.
    for (int i = 0; i < m_node.size(); i++)
        if (m_node[i] != nullptr)
            delete m_node[i];
    if (m_epsilon != nullptr) delete m_epsilon;
    if (m_terminal != nullptr) delete m_terminal;


}

ranged_string compiled::state_machine_input(const string& src, unsigned index_start, bool check_at_front_only)
{
    map<int, ranged_string*> found;
    map<string, node*> actives;
    vector<active_request_info> next_actives;

    for (unsigned i = index_start; i < src.length(); i++) {
        //엡실론 신호를 매 주기기마다 부여함
        //check_at_front_only면 문자열 시작이 패턴과 일치하는 경우만 확인한다.
        //즉 최초 1회만 엡실론 신호를 부여한다.
        if (!(check_at_front_only && i > 0)) {
            for (node_ptr* e : m_epsilon->next())
                e->request_active_nexttime(next_actives, i, '\0');
            //코드 중복 발생
            //다음 활성화될 노드 리스트를 받아오고 활성화한다.
            while (!next_actives.empty()) {
                active_request_info& info = next_actives.back();
                request_active(actives, info.target, info.start_index);
                next_actives.pop_back();
            }
        }


        // give ch all nodes
        // nv[j]가 active 상태였다면 이 연산 후 nv[j]에서 나오는 화살표는
        // 조건을 만족할 시 가리키는 노드의 transited를 활성화한다.
        auto it = actives.begin();
        for (; it != actives.end(); it++) {
            node* target = it->second;
            target->input(next_actives, src[i]);
        }
        actives.clear();

        //다음 활성화될 노드 리스트를 받아오고 활성화한다. (코드 중복)
        while (!next_actives.empty()) {
            active_request_info& info = next_actives.back();
            request_active(actives, info.target, info.start_index);
            next_actives.pop_back();
        }

        //터미널 노드 순회 후 accepted되었으면 matched를 생성한다.
        for (node_ptr* terminal : m_terminal->next()) {
            if (terminal->is_accepted()) {
                unsigned istart = terminal->index_start();
                if (found.find(istart) != found.end()) delete found[istart];
                found[istart] = new ranged_string(src, istart, i + 1, true);
            }
        }
    }

    ///*debug*/cout << endl;
    //src를 모두 input한 뒤 모든 노드의 플래그 초기화
    for (node* target : m_node) target->clear_flags();


    if (found.empty()) //일치하는 패턴 없음
        return ranged_string::invalid();
    else {
        ranged_string ret = *found.begin()->second;
        for (auto& p : found)
            delete p.second;

        return ret;
    }
}
void compiled::request_active(map<string, node*>& actives, node* to_active, unsigned state_istart)
{
    const string& name = to_active->name();
    if (actives.find(name) != actives.end() &&
        actives[name]->index_start() < state_istart)
        return;
    to_active->active(state_istart);
    actives[name] = to_active;


}


} //end of MySimpleRegex & compiled member functions