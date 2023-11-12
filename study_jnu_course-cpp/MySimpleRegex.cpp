#include "MySimpleRegex.h"
#include <map>

namespace assignment1 { 
using matched = MySimpleRegex::matched;
using compiled = MySimpleRegex::compiled;


/*******************   MySimpleRegex static 함수   *******************/
compiled MySimpleRegex::compile(const string& m_regex, int test = 0) {
    return compiled(m_regex, test);
}
matched MySimpleRegex::match(const string& m_regex, const string& source) {
    return compiled(m_regex).match(source);
}
vector<matched> MySimpleRegex::match_all(const string m_regex, const string& source) {
    return compiled(m_regex).match_all(source);
}



/*******************   문자 매치 클래스   *******************/
class compiled::Imatchable {
public:
    virtual bool test(char ch) = 0;
};
    
class compiled::match_single : public compiled::Imatchable {
private: char ch;
public:
    match_single(char ch) : ch(ch) {}
    virtual bool test(char _ch) override { return ch == _ch; }
};

class compiled::match_dot : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return ('A' <= _ch && _ch <= 'Z') || ('a' <= _ch && _ch <= 'z');
    }
};



/*******************   node 및 파생 클래스   *******************/
//처음으로 활성화된 상태였을 때 첫 인덱스를 반환한다.
//is_accepted일 때 인식된 문자열의 첫 위치를 의미한다.
unsigned compiled::node::index_start() const { return m_state_istart; }

//본 노드가 가리키는 노드를 추가한다.
void compiled::node::addNode(node_ptr* _next) {
    next.push_back(_next);
    _next->link_reverse_ref(this);
}

//역링크를 추가한다.
void compiled::node::add_link_reverse_ref(node* ref) {
    reverse_ref.push_back(ref);
}

//노드를 활성화 상태로 만든다.
void compiled::node::active(int state_istart) {
    m_state_istart = state_istart;
    m_state_active = true;
}

//노드 상태와 관련된 값과 플래그들을 모두 클리어한다.
void compiled::node::clear_flags() {
    m_state_istart = 0;
    m_state_active = false;

    for (node_ptr* p : next)
        p->clear_flag();
}

//본 노드에서 accept 되었는지 확인한다.
bool compiled::node::is_accepted() const {
    return m_is_terminal && m_state_active;
}

//노드에 한 문자를 입력함: 노드가 활성화됐을 때 문자가 매칭되면 다음 노드들에게 활성화를 요청함
//노드 활성화 & 문자 매칭일 때 터미널 노드인 경우 is_accepted가 true가 됨
void compiled::node::input(vector<active_request_info>& next_active, const char ch) {
    if (!m_state_active) return;

    for (int i = 0; i < next.size(); i++) {
        next[i]->active_request(next_active, m_state_istart, ch);
    }

    //인식되었을 경우 true
    //m_isAccepted = m_state_active && match && m_isTerminal;
    m_state_active = false; // deactivate this node after matching
}



//node_ptr에 담긴 matcher를 반환한다.
compiled::node_ptr::~node_ptr() { if (matcher != nullptr) delete matcher; }

//연결된 노드에 active를 요청한다. 한번에 될 수도 있고, 그렇지 않을 수도 있음
void compiled::node_ptr::active_request(vector<active_request_info>& next_active, unsigned state_istart, char ch) {
    //요구되는 문자와 일치되는 경우에만 액션을 취함
    if (matcher->test(ch)) transition_action(next_active, state_istart);
}



/*******************   compiled 클래스 함수   *******************/
//TODO: 주어진 정규표현식으로 상태머신 생성하기 필요
    //상태기계 노드 컨테이너에 상태기계를 생성한다.
void compiled::create_state_machine(int test) {

    //mock implementation
    //문법을 해석해 아래 작업이 알아서 되어야함
    if (test == 1) {
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
        //새로운 노드포인터!!
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

//생성된 상태기계를 삭제한다.
void compiled::delete_state_machine() {
    for (int i = 0; i < m_node.size(); i++)
        if (m_node[i] != nullptr)
            delete m_node[i];
}

//TODO: T* , T+ 같은 케이스에서 그리디하게 동작하게 바꿔야 함
//상태기계에 문자열을 입력으로 받고, 가장 처음으로 accept된 일치 정보를 출력한다.
matched compiled::state_machine_input(const string& src, unsigned index_start, bool check_at_front_only) {
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


} //end of namespace assignment1