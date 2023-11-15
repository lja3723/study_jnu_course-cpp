#include "MySimpleRegex.h"

namespace assignment1 { 
using compiled = MySimpleRegex::compiled;


/*******************   MySimpleRegex static 함수   *******************/
compiled MySimpleRegex::compile(const string& m_regex, interpret_mode mode) {
    return compiled(m_regex, mode);
}
ranged_string MySimpleRegex::match(const string& m_regex, const string& source, interpret_mode mode) {
    return compiled(m_regex, mode).match(source);
}
vector<ranged_string> MySimpleRegex::match_all(const string m_regex, const string& source, interpret_mode mode) {
    return compiled(m_regex, mode).match_all(source);
}



/*******************   문자 매치 및 파생 클래스   *******************/
class compiled::Imatchable {
public:
    virtual bool test(char ch) = 0;
};   
class compiled::matcher_true : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return true;
    }
};
class compiled::matcher_not : public compiled::Imatchable {
private:
    Imatchable* m;
public:
    matcher_not(Imatchable* to_not) : m(to_not) {}
    ~matcher_not() { if (m != nullptr) delete m; }
    virtual bool test(char _ch) override {
        return !m->test(_ch);
    }
};
class compiled::matcher_combine : public compiled::Imatchable {
private:
    vector<Imatchable*> m;
public:
    matcher_combine(initializer_list<Imatchable*> to_combine) {
        for (Imatchable* c : to_combine)
            m.push_back(c);
    }
    ~matcher_combine() {
        for (Imatchable* c : m)
            if (c != nullptr) delete c;
    }
    virtual bool test(char _ch) override {
        bool ret = false;
        for (Imatchable* c : m)
            ret |= c->test(_ch);
        return ret;
    }
};
class compiled::matcher_range : public compiled::Imatchable {
private:
    char _start, _end;
public:
    matcher_range(char _ch_start, char _ch_end) : _start(_ch_start), _end(_ch_end) {}
    virtual bool test(char _ch) override {
        return _start <= _ch && _ch <= _end;
    }
};
class compiled::matcher_single : public compiled::Imatchable {
private: char ch;
public:
    matcher_single(char ch) : ch(ch) {}
    virtual bool test(char _ch) override { return ch == _ch; }
};
class compiled::matcher_alphabet : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return matcher_combine({
            new matcher_range('A', 'Z'),
            new matcher_range('a', 'z')}
        ).test(_ch);
    }
};
class compiled::matcher_number : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return matcher_range('0', '9').test(_ch);
    }
};
class compiled::matcher_word : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return matcher_combine({
            new matcher_alphabet(),
            new matcher_number(),
            new matcher_single('_')
            }).test(_ch);
    }
};
class compiled::matcher_space : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        string s = "\t\n\v\f\r ";
        bool ret = false;
        for (char c : s) ret |= c == _ch;
        return ret;
    }
};
class compiled::matcher_dot : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return matcher_not(new matcher_single('\n')).test(_ch);
    }
};



/*******************   node 및 파생 클래스   *******************/
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

    //연결된 노드가 다음 사이클에 active 되도록 요청한다.
    //한번에 될 수도 있고, 그렇지 않을 수도 있음
    virtual void request_active_nexttime(
        vector<active_request_info>& next_active,
        unsigned state_istart, char ch 
    ) final;


}; /************************ noe_ptr end ************************/
class compiled::node_ptr_direct : public node_ptr {
protected:
    virtual void transition_action(vector<active_request_info>& next_active, unsigned state_istart) override {
        next_active.push_back({ m_pNode, state_istart });
    }

public:
    node_ptr_direct(Imatchable* matcher, node* pNode) : node_ptr(matcher, pNode) {}


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


}; /************************ node_ptr_cnt_inner end ************************/



/**************   node 클래스 멤버함수   **************/
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
void compiled::node_ptr::request_active_nexttime(
    vector<active_request_info>& next_active, unsigned state_istart, char ch) {
    //요구되는 문자와 일치되는 경우에만 전이 행동을 취한다.
    if (matcher->test(ch)) 
        transition_action(next_active, state_istart);
}



/******   state machine creator 및 파생 클래스   ******/
//TODO: 주어진 정규표현식으로 상태머신 생성하기 필요
class compiled::state_machine_creator {
public:
    enum state {
        init,
        asterisk,
        plus,
        range,
        range_end,
        and_oper,
        escape,
        number,
        character
    };

    state_machine_creator(vector<node*>& _node, node*& _epsilon, vector<node*>& _terminal) :
        m_node(_node), m_epsilon(_epsilon), m_terminal(_terminal),
        m_prev_state(init), m_cur_state(init), m_cur_idx(0), m_regex("") {}

    virtual bool make(const string& regex) final {
        //상태 머신을 초기화한다.
        m_regex = regex;
        clear();
        m_epsilon = new node("epsilon");

        //정규표현식을 한 문자씩 읽으면서 작업을 수행한다.
        bool m_syntax_correct = true;
        for (int i = 0; i < m_regex.size(); i++) {
            m_cur_state = interpret(m_regex[i]);
            if (!action()/* 파생되는 클래스마다 다른 동작 */) {
                m_syntax_correct = false;
                break;
            }
        }

        //최종적인 마무리 작업을 수행한다.
        m_syntax_correct &= final_action()/* 파생되는 클래스마다 다른 동작 */;
        if (!m_syntax_correct) clear(); //잘못 생성된 상태머신을 초기화한다.
        return m_syntax_correct;
    }

private:
    //현재 문자로 상태를 추측한다.
    state interpret(char c) {
        switch (c) {
        case '*': return asterisk;
        case '+': return plus;
        case '{': return range;
        case '}': return range_end;
        case '|': return and_oper;
        case '\\': return escape;
        }
        if ('0' <= c && c <= '9')  return number;
        return character;
    }
    
    //노드 컨테이너를 초기화한다.
    void clear() {
        for (int i = 0; i < m_node.size(); i++)
            if (m_node[i] != nullptr)
                delete m_node[i];
        if (m_epsilon != nullptr) delete m_epsilon;

        m_node.clear();
        m_epsilon = nullptr;
    }


protected:
    //외부 참조 변수
    //이 변수를 조작해 상태 머신을 생성한다.
    vector<node*>& m_node;
    node*& m_epsilon;
    vector<node*>& m_terminal;

    //내부 변수
    state m_prev_state;
    state m_cur_state;
    int m_cur_idx;
    string m_regex;

    //엡실론을 받을 노드를 등록한다.
    void m_add_to_epsilon(node* target) {
        m_epsilon->add_link(new node_ptr_direct(new matcher_true(), target));
    }

    virtual bool action() = 0;
    virtual bool final_action() = 0;


};
class compiled::state_machine_creator_for_assignment1 : public state_machine_creator {
public:
    state_machine_creator_for_assignment1(
        vector<node*>& _node, node*& _epsilon, vector<node*>& _terminal) :
            state_machine_creator(_node, _epsilon, _terminal), node_count(0) {}

private:
    int node_count;

    bool syntax_error(string prev_state) {
        cout << "syntax err; prev state was " << prev_state << ";\n";
        return false;
    }
    bool action_init() {
        new_node_back();
        m_add_to_epsilon(m_node.back());

        if (m_cur_state == number || m_cur_state == character) {
            m_node.push_back(new node("s"));
            new_node_back();
            m_node[m_node.size() - 2]->add_link(new node_ptr_direct(
                new matcher_single(m_regex[m_cur_idx]), m_node.back()));

            m_prev_state = m_cur_state;
            return true;
        }

        if (m_cur_state == escape) {

        }
        
        return syntax_error("init");
    }
    bool action_escape() {

        return true;
    }
    bool action_number() {
        return true;
    }
    bool action_character() {
        return true;
    }
    bool action_asterisk() {
        return true;
    }
    bool action_plus() {
        return true;
    }
    bool action_range() {
        return true;
    }
    bool action_range_end() {
        return true;
    }
    bool action_and_oper() {
        return true;
    }

    void new_node_back() {
        m_node.push_back(new node("s" + ++node_count));
    }

protected:
    virtual bool action() override {
        //이전 상태에 따라 다른 작업이 수행된다.
        switch (m_prev_state) {
        case init:      return action_init();
        case asterisk:  return action_asterisk();
        case plus:      return action_plus();
        case range:     return action_range();
        case range_end: return action_range_end();
        case and_oper:  return action_and_oper();
        case escape:    return action_escape();
        case number:    return action_number();
        case character: return action_character();
        default: return false;
        }
    }
    virtual bool final_action() override {

        return true;
    }


};
class compiled::state_machine_creator_for_general : public state_machine_creator {
public:
    state_machine_creator_for_general(vector<node*>& _node, node*& _epsilon, vector<node*>& _terminal) :
        state_machine_creator(_node, _epsilon, _terminal) {}

protected:
    virtual bool action() override {
        return true;
    }
    virtual bool final_action() override {
        //mock implementation
        //문법을 해석해 아래 작업이 알아서 되어야함
        if (m_regex == "{test}") {
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->add_link(new node_ptr_direct(
                new matcher_alphabet(), m_node[0]));
            m_node[0]->add_link(new node_ptr_direct(
                new matcher_alphabet(), m_node[1]));

            m_add_to_epsilon(m_node[0]);
            m_terminal.push_back(m_node[1]);
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
            m_terminal.push_back(m_node[3]);
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

            m_terminal.push_back(m_node[3]);
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
            m_terminal.push_back(m_node[11]);
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
            m_terminal.push_back(m_node[3]);
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
            m_terminal.push_back(m_node[3]);
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
            m_terminal.push_back(m_node[3]);
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
            m_terminal.push_back(m_node[3]);
        }
        else if (m_regex == "T{3,5}") {
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[0]));
            m_node[0]->add_link(new node_ptr_inner_counter(new matcher_single('T'), m_node[1], 3, 5));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            m_terminal.push_back(m_node[1]);
        }
        else if (m_regex == "T+") {
            m_node.resize(2);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1", true);

            m_node[0]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[1]));
            m_node[0]->add_link(new node_ptr_direct(new matcher_single('T'), m_node[0]));

            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));
            m_terminal.push_back(m_node[1]);
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

            m_add_to_epsilon(m_node[0]);
            m_terminal.push_back(m_node[2]);
        }


        return true;


    }


};



/*******************   compiled 클래스 멤버함수   *******************/
compiled::compiled(const string& m_regex, interpret_mode mode)
    : m_regex(m_regex), m_mode(mode), m_epsilon(nullptr) {
    //해석 모드에 따라 서로 다른 상태머신이 생성된다.
    bool syntax_check = true;
    if (m_mode == general) {
        state_machine_creator_for_general creator(m_node, m_epsilon, m_terminal);
        syntax_check = creator.make(m_regex);
    }
    else if (m_mode == assignment1) {
        state_machine_creator_for_assignment1 creator(m_node, m_epsilon, m_terminal);
        syntax_check = creator.make(m_regex);
    }

    //정규식 문법이 잘못된 경우 빈 상태 기계가 생성된다.
    if (!syntax_check) {
        cout << "정규표현식 문법이 잘못되었습니다. 비교가 제대로 수행되지 않을 수 있습니다." << endl;
    }


}
compiled::~compiled() { 
    //동적 할당된 노드를 삭제한다.
    for (int i = 0; i < m_node.size(); i++)
        if (m_node[i] != nullptr)
            delete m_node[i];
    if (m_epsilon != nullptr) delete m_epsilon;


}


ranged_string compiled::state_machine_input(
    const string& src, unsigned index_start, bool check_at_front_only) 
{
    map<int, ranged_string*> found;
    map<string, node*> actives;
    vector<active_request_info> next_actives;

    ///*debug*/cout << "test for \"" << source << "\"" << endl;
    for (unsigned i = index_start; i < src.length(); i++) {
        ///*debug*/cout << "  >> input " << src[i] << endl;
        //엡실론 신호를 매 주기기마다 부여함
        //check_at_front_only면 문자열 시작이 패턴과 일치하는 경우만 확인한다.
        //즉 최초 1회만 엡실론 신호를 부여한다.
        if ( !(check_at_front_only && i > 0) ) {
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
        for (node* terminal : m_terminal) {
            if (terminal->is_accepted()) {
                ///*debug*/cout << "       active_count: " << nv[j].active_count() << endl;
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

void compiled::request_active(
    map<string, node*>& actives, node* to_active, unsigned state_istart) 
{
    const string& name = to_active->name();
    if (actives.find(name) != actives.end() &&
        actives[name]->index_start() < state_istart)
        return;
    to_active->active(state_istart);
    actives[name] = to_active;

    
}


} //end of namespace assignment1