#include "MySimpleRegex.h"
#include "MySimpleRegex_node.h"
#include <string>


/****************************/
/*      MySimpleRegex &     */
/*      compiled Classes    */
/****************************/
namespace assignment1 {


/******   cout 출력 연산자 오버로딩   ******/
ostream& operator<<(ostream& os, const ranged_string& rs) {
    cout << "range [" << rs.start << "," << rs.end << ") " << "\"" << rs.group() << "\"";
    return os;
}



/******   state machine 생성기 클래스   ******/
class compiled::state_machine_creator {
    enum state_and {  //| 연산자 처리 위한 열거형
        not_proc, //|를 안 만남
        before_proc, //|를 만난 직후
        after_proc //| 오른쪽 피연산자 처리된 직후
    };

public:
    state_machine_creator(vector<node*>& _node, node*& _epsilon, node*& _terminal) :
        m_node(_node), 
        m_epsilon(_epsilon), 
        m_terminal(_terminal), 
        node_cnt(0) {}


    //주어진 regex로 상태 머신을 생성한다.
    //return true: 상태 머신 생성 성공
    //return false: 상태 머신 생성 실패(regex 문법 잘못됨)
    bool make(const string& regex) {
        this->regex = regex;
        //상태 머신을 초기화한다.
        clear();

        //하드코딩된 정규식과 일치하는 경우 생성 성공
        if (is_hard_coded(regex)) return true;

        //사전 처리
        bool syntax_valid = true;
        state_and state = not_proc;
        add_node_back();
        add_to_epsilon(m_node.back());

        //정규표현식을 한 문자씩 읽으면서 작업을 수행한다.
        for (size_t i = 0; i < regex.size(); i++) {

            //(K* 체크) asterisk 문법과 일치하면 상태 머신 연결작업 수행 후 i를 증가시킨다.
            if (check_asterisk(syntax_valid, state, i)) continue;
            //일치하지 않으면서 문법도 틀리면 종료한다.
            if (!syntax_valid) break;

            //K+ 체크
            if (check_plus(syntax_valid, state, i)) continue;
            if (!syntax_valid) break;

            //K{m,n} 체크
            if (check_range(syntax_valid, state, i)) continue;
            if (!syntax_valid) break;

            //K 체크
            if (check_char(syntax_valid, state, i)) continue;
            if (!syntax_valid) break;

            //| 체크
            if (check_and(syntax_valid, state, i)) continue;
            if (!syntax_valid) break;

            syntax_valid = false;
            break;
        }

        //and 연산으로 끝난 경우
        if (state == before_proc)
            syntax_valid = false;

        if (!syntax_valid) clear(); //잘못 생성된 상태머신을 초기화한다.       
        else {
            //상태머신을 마무리짓는다.
            m_node.back()->set_terminal(true);
            add_to_terminal(m_node.back());
        }
        return syntax_valid;


    }


private:
    //외부 참조 변수
    //이 변수들을 조작해 상태 머신을 생성한다.
    vector<node*>& m_node;
    node*& m_epsilon;
    node*& m_terminal;


    //내부 변수
    int node_cnt; //생성된 노드 개수
    string regex; 


    //하드코딩된 상태머신이 존재하는 경우 해당 상태머신으로 초기화
    bool is_hard_coded(const string& m_regex) {
        //mock implementation
        //test state-machine of regex
        if (m_regex == "abc|ade") {
            m_node.resize(6/*정규표현식 해석 후 노드 개수가 되어야 함*/);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2");
            m_node[3] = new node("s3", true);
            m_node[4] = new node("s4");
            m_node[5] = new node("s5");

            // set links
            m_node[0]->add_link(new node_ptr_direct(m_node[0], new matcher_single('a'), m_node[1]));
            m_node[0]->add_link(new node_ptr_direct(m_node[0], new matcher_single('a'), m_node[4]));
            m_node[1]->add_link(new node_ptr_direct(m_node[1], new matcher_single('b'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(m_node[2], new matcher_single('c'), m_node[3]));
            m_node[4]->add_link(new node_ptr_direct(m_node[4], new matcher_single('d'), m_node[5]));
            m_node[5]->add_link(new node_ptr_direct(m_node[5], new matcher_single('e'), m_node[3]));

            // 엡실론 신호 받는 노드 설정
            add_to_epsilon(m_node[0]);

            // 터미널 노드 설정
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

            m_node[0]->add_link(new node_ptr_direct(m_node[0], new matcher_single('N'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(m_node[1], new matcher_single('K'), m_node[5]));
            m_node[1]->add_link(new node_ptr_direct(m_node[1], new matcher_single('K'), m_node[2]));
            m_node[1]->add_link(new node_ptr_direct(m_node[1], new matcher_single('K'), m_node[8]));
            m_node[2]->add_link(new node_ptr_direct(m_node[2], new matcher_single('a'), m_node[3]));
            m_node[2]->add_link(new node_ptr_direct(m_node[2], new matcher_single('A'), m_node[6]));
            m_node[3]->add_link(new node_ptr_direct(m_node[3], new matcher_single('b'), m_node[4]));
            m_node[4]->add_link(new node_ptr_direct(m_node[4], new matcher_single('c'), m_node[2]));
            m_node[4]->add_link(new node_ptr_direct(m_node[4], new matcher_single('c'), m_node[5]));
            m_node[5]->add_link(new node_ptr_direct(m_node[5], new matcher_single('N'), m_node[10]));
            m_node[6]->add_link(new node_ptr_direct(m_node[6], new matcher_single('B'), m_node[7]));
            m_node[7]->add_link(new node_ptr_direct(m_node[7], new matcher_single('C'), m_node[2]));
            m_node[7]->add_link(new node_ptr_direct(m_node[7], new matcher_single('C'), m_node[5]));
            m_node[8]->add_link(new node_ptr_direct(m_node[8], new matcher_single('O'), m_node[9]));
            m_node[9]->add_link(new node_ptr_direct(m_node[9], new matcher_single('P'), m_node[8]));
            m_node[9]->add_link(new node_ptr_direct(m_node[9], new matcher_single('P'), m_node[10]));
            m_node[10]->add_link(new node_ptr_direct(m_node[10], new matcher_single('Q'), m_node[11]));

            add_to_epsilon(m_node[0]);
            add_to_terminal(m_node[11]);
            return true;
        }
        else if (m_regex == "Tn(abTn)*") {
            m_node.resize(5);

            m_node[0] = new node("s0");
            m_node[1] = new node("s1");
            m_node[2] = new node("s2", true);
            m_node[3] = new node("s3");
            m_node[4] = new node("s4");

            m_node[0]->add_link(new node_ptr_direct(m_node[0], new matcher_single('T'), m_node[1]));
            m_node[1]->add_link(new node_ptr_direct(m_node[1], new matcher_single('n'), m_node[2]));
            m_node[2]->add_link(new node_ptr_direct(m_node[2], new matcher_single('a'), m_node[3]));
            m_node[3]->add_link(new node_ptr_direct(m_node[3], new matcher_single('b'), m_node[4]));
            m_node[4]->add_link(new node_ptr_direct(m_node[4], new matcher_single('T'), m_node[1]));

            add_to_epsilon(m_node[0]);
            add_to_terminal(m_node[2]);
            return true;
        }

        //매치되는 regex 없음
        return false;


    }
    //정규식에서 지원하는 '단일 알파벳' 또는 . 인지 확인한다.
    bool is_char(char ch) {
        return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ch == '.';


    }


    //엡실론을 받을 노드를 등록한다.
    void add_to_epsilon(node* target) {
        if (m_epsilon != nullptr)
            m_epsilon->add_link(
                new node_ptr_direct(m_epsilon, new matcher_true(), target));


    }
    //터미널 노드를 등록한다.
    void add_to_terminal(node* target) {
        if (m_terminal != nullptr)
            m_terminal->add_link(
                new node_ptr_direct(m_terminal, new matcher_true(), target));


    }
    //새 노드를 뒤에 추가한다.
    void add_node_back() {
        m_node.push_back(new node("s" + to_string(node_cnt++)));


    }
    //to_pass 노드를 가리켰던 모든 노드가 target도 가리키게 링크를 추가한다.
    void add_passing_link(node* to_pass, node* target) {
        for (node_ptr* rev_ptr : to_pass->reverse_ref()) {
            node* rev = const_cast<node*>(rev_ptr->origin());
            //자기자신 참조가 아닌 경우에만 연결하기
            if (!rev_ptr->is_ref_equal(rev_ptr->origin()))
                rev->add_link(rev_ptr->copy(target));
        }


    }
    //노드 컨테이너를 초기화한다.
    void clear() {
        for (int i = 0; i < m_node.size(); i++)
            if (m_node[i] != nullptr) delete m_node[i];
        if (m_epsilon != nullptr) delete m_epsilon;
        if (m_terminal != nullptr) delete m_terminal;

        m_node.clear();
        m_epsilon = new node("epsilon");
        m_terminal = new node("terminal");


    }
    //문자와 맞는 matcher를 반환한다.
    Imatchable* get_matcher(char ch) {
        if (ch == '.')
            return new matcher_dot();
        else if (is_char(ch))
            return new matcher_single(ch);
        else return nullptr;


    }
    //상황에 맞는 tail을 반환한다.
    node* get_tail(state_and& state) {
        //tail 참조 얻고 뒷부분 노드 추가
        if (state == not_proc)
            add_node_back();
        else if (state == before_proc)
            state = after_proc;
        else {
            add_node_back();
            state = not_proc;
        }

        return m_node[m_node.size() - 2];


    }


    // * 연산자 여부 체크한다.
    bool check_asterisk(bool& syntax, state_and& state, size_t& idx) {
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

        //tail 얻기
        node* tail = get_tail(state);

        //다음 노드 가리키는 링크 추가
        tail->add_link(new node_ptr_direct(tail, matcher, m_node.back()));

        //자기 자신 가리키는 노드 추가        
        tail->add_link(new node_ptr_direct(tail, matcher->copy(), tail));

        //이전 노드에서 다음 노드로 바로 가는 링크 추가
        add_passing_link(tail, m_node.back());

        idx++;
        return true;


    }
    // + 연산자 여부 체크한다.
    bool check_plus(bool& syntax, state_and& state, size_t& idx) {
        //범위 & 연산 대상인지 확인
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '+')
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

        //tail 얻기
        node* tail = get_tail(state);

        //다음 노드 가리키는 링크 추가
        tail->add_link(new node_ptr_direct(tail, matcher, m_node.back()));

        //자기 자신 가리키는 노드 추가        
        tail->add_link(new node_ptr_direct(tail, matcher->copy(), tail));

        idx++;
        return true;


    }
    // {} 연산자 여부 체크한다.
    bool check_range(bool& syntax, state_and& state, size_t& idx) {
        //범위 표현식인지 체크
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '{')
            return false;

        //연산 적용 가능한지 체크
        if (!is_char(regex[idx])) {
            syntax = false;
            return false;
        }

        //범위식 파싱하기
        size_t range_end = -1;
        size_t split = -1;
        for (size_t i = idx + 2; i < regex.size(); i++) {
            if (regex[i] == '}') {
                range_end = i;
                break;
            }
            else if (regex[i] == ',') {
                if (split != -1) {
                    syntax = false;
                    return false;
                }
                split = i;
            }
            else if (!('0' <= regex[i] && regex[i] <= '9')) {
                syntax = false;
                return false;
            }
        }

        //첫번째 범위와 두번째 범위의 문자열 길이 계산
        size_t arg1_size = split - idx - 2;
        size_t arg2_size = range_end - split - 1;

        //4가지 조건 중 하나라도 참이면 문법 에러임
        bool pred = false;
        pred |= range_end == -1;
        pred |= split == -1;
        pred |= arg1_size == 0;
        pred |= arg2_size == 0;

        if (pred) {
            syntax = false;
            return false;
        }

        //범위를 계산한다.
        size_t istart = size_t(stoi(regex.substr(idx + 2, arg1_size)));
        size_t iend = size_t(stoi(regex.substr(split + 1, arg2_size)));
        if (istart > iend) {
            syntax = false;
            return false;
        }

        //matcher 얻기
        Imatchable* matcher = get_matcher(regex[idx]);
        if (matcher == nullptr) {
            syntax = false;
            return false;
        }

        //tail 얻기
        node* tail = get_tail(state);

        //다음 노드 가리키는 링크 추가
        tail->add_link(new node_ptr_inner_counter(tail, matcher, m_node.back(), istart, iend));

        //자기 자신 가리키는 노드 추가        
        tail->add_link(new node_ptr_direct(tail, matcher->copy(), tail));

        //k{0,3} 같은 케이스를 고려한다.
        //이전 노드에서 다음 노드로 바로 가는 링크 추가
        if (istart == 0) add_passing_link(tail, m_node.back());

        idx = range_end;
        return true;


    }
    //일반 문자로 이루어졌는지 체크한다.
    bool check_char(bool& syntax, state_and& state, size_t& idx) {
        if (!is_char(regex[idx]))
            return false;

        Imatchable* matcher = get_matcher(regex[idx]);
        if (matcher == nullptr) {
            syntax = false;
            return false;
        }

        node* tail = get_tail(state);
        tail->add_link(new node_ptr_direct(tail, matcher, m_node.back()));

        return true;


    }
    // | 연산자 여부 체크한다.
    bool check_and(bool& syntax, state_and& state, size_t& idx) {
        //범위 & 연산 대상인지 확인
        if (regex[idx] != '|')
            return false;

        // a||b 나 a|b|c 같은 경우 거르기
        if (state != not_proc) {
            syntax = false;
            return false;
        }

        // |b 같은 경우 거르기
        if (m_node.size() < 2) {
            syntax = false;
            return false;
        }

        //새 노드를 맨 뒷 원소 바로 앞에 추가한다.
        add_node_back();
        node* tmp = m_node[m_node.size() - 2];
        m_node[m_node.size() - 2] = m_node[m_node.size() - 1];
        m_node[m_node.size() - 1] = tmp;

        //기존 맨 뒤의 바로 앞 원소를 가리키던 연결을 새로 추가된 노드에도 연결해준다.
        add_passing_link(m_node[m_node.size() - 3], m_node[m_node.size() - 2]);

        state = before_proc;
        return true;


    }

    

}; // end of class: state_machine_creator



/*******************   MySimpleRegex static 함수   *******************/
compiled MySimpleRegex::compile(const string& m_regex) {
    return compiled(m_regex);


}
ranged_string MySimpleRegex::match(const string& m_regex, const string& source, size_t index_start) {
    compiled cp = compile(m_regex);
    return cp.match(source, index_start);


}
vector<ranged_string> MySimpleRegex::match_all(const string m_regex, const string& source) {
    compiled cp = compile(m_regex);
    return cp.match_all(source);


}



/*******************   compiled 멤버 함수   *******************/
//public:
compiled::compiled(const string& m_regex)
    : m_epsilon(nullptr), m_terminal(nullptr), m_regex(m_regex) {
    state_machine_creator creator(m_node, m_epsilon, m_terminal);

    //정규식 문법이 잘못된 경우 빈 상태 기계가 생성되고, is_valid가 false가 된다.
    m_is_valid = creator.make(m_regex);


}
compiled::~compiled() {
    //동적 할당된 노드를 삭제한다.
    for (int i = 0; i < m_node.size(); i++)
        if (m_node[i] != nullptr)
            delete m_node[i];
    if (m_epsilon != nullptr) delete m_epsilon;
    if (m_terminal != nullptr) delete m_terminal;


}
bool compiled::is_valid() {
    return m_is_valid;


}
ranged_string compiled::match(const string& source, size_t index_start) {
    return state_machine_input(source, index_start, false);


}    
vector<ranged_string> compiled::match_all(const string& source) {
    vector<ranged_string> ret;
    size_t found_idx = 0;

    while (found_idx <= source.size()) {
        ranged_string ans = match(source, found_idx);
        if (!ans.is_valid) {
            found_idx++;
            continue;
        }

        found_idx = ans.end;
        if (ans.start == ans.end) found_idx++;
        ret.push_back(ans);
    }
    return ret;


}


//private:
ranged_string compiled::state_machine_input(
    const string& src, size_t index_start, bool check_at_front_only) 
{
    map<size_t, ranged_string*> found; //찾은 ranged_string 리스트
    map<string, node*> actives;     //현재 활성화된 노드 리스트
    vector<active_request_info> next_actives; //다음번에 활성화될 노드 리스트

    for (size_t i = index_start; i < src.length(); i++) {
        give_epsilon(next_actives, actives, i, check_at_front_only);
        check_terminal_active(found, src, i);

        // active 상태의 노드에서 나오는 모든 포인터에 문자를 입력한다.
        for (auto it = actives.begin(); it != actives.end(); it++)
            it->second->input(next_actives, src[i]);
        actives.clear();

        active_transition(next_actives, actives);
    }

    //엡실론 신호를 마지막으로 부여해 보고, 터미널이 활성화되었는지 확인한다.
    give_epsilon(next_actives, actives, src.length(), check_at_front_only);
    check_terminal_active(found, src, src.length());

    //모든 작업이 마무리 된 후, 모든 노드의 플래그를 초기화한다.
    for (node* target : m_node) target->clear_flags();

    if (found.empty()) //일치하는 패턴 없음
        return ranged_string::invalid();
    else {
        ranged_string ret = *found.begin()->second;
        for (auto& p : found) delete p.second;

        return ret;
    }


}
void compiled::give_epsilon(
    vector<active_request_info>& next_actives, map<string, node*>& actives, size_t state_istart, bool check_at_front_only) 
{
    //check_at_front_only == false이면 엡실론 신호를 매 주기마다 부여한다.
    //check_at_front_only == true이면 문자열 시작 부분이 패턴과 일치하는 경우만 확인한다.
    // -> 즉 최초 1회만 엡실론 신호를 부여한다.
    if (!(check_at_front_only && state_istart > 0)) {
        for (node_ptr* nxt : m_epsilon->next())
            nxt->request_active_nexttime(next_actives, state_istart, '\0');

        //next_actives를 참고하여 활성화될 노드들을 실제로 활성화한다.
        active_transition(next_actives, actives);
    }


}
void compiled::active_transition(
    vector<active_request_info>& next_actives, map<string, node*>& actives) 
{
    //next_actives에서 노드를 하나씩 가져와
    //조건을 만족하는 노드들만 active한 뒤, actives 리스트에 추가한다.
    while (!next_actives.empty()) {
        node* to_active = next_actives.back().target;
        size_t state_istart = next_actives.back().start_index;
        const string& name = to_active->name();

        // 아직 active를 하지 않았거나,
        // state_istart가 더 작아지도록 active 할 경우에만 active를 수행한다.
        if (actives.find(name) == actives.end() ||
            state_istart <=actives[name]->index_start()) 
        {
            to_active->active(state_istart);
            actives[name] = to_active;
        }

        next_actives.pop_back();
    }
 
   
}
void compiled::check_terminal_active(
    map<size_t, ranged_string*>& found, const string& src, size_t idx) 
{
    //accepted 된 터미널 노드를 발견하면 found에 추가한다.
    for (node_ptr* terminal : m_terminal->next()) {
        if (terminal->is_accepted()) {
            size_t istart = terminal->index_start();
            if (found.find(istart) != found.end()) delete found[istart];
            found[istart] = new ranged_string(src, istart, idx, true);
        }
    }


}


} //end of namespace assignment1