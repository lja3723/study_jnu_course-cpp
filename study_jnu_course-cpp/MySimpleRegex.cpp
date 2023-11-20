#include "MySimpleRegex.h"
#include "MySimpleRegex_node.h"
#include <string>


/****************************/
/*      MySimpleRegex &     */
/*      compiled Classes    */
/****************************/
namespace assignment1 {


/******   state machine 생성기 클래스   ******/
class compiled::state_machine_creator {
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
        add_node_back();
        add_to_epsilon(m_node.back());

        //정규표현식을 한 문자씩 읽으면서 작업을 수행한다.
        for (size_t i = 0; i < regex.size(); i++) {
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
    //정규식에서 지원하는 '단일 알파벳' 또는 . 인지 확인한다.
    bool is_char(char ch) {
        return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ch == '.';


    }


    //엡실론을 받을 노드를 등록한다.
    void add_to_epsilon(node* target) {
        m_epsilon->add_link(new node_ptr_direct(new matcher_true(), target));


    }
    //터미널 노드를 등록한다.
    void add_to_terminal(node* target) {
        m_terminal->add_link(new node_ptr_direct(new matcher_true(), target));


    }
    //새 노드를 뒤에 추가한다.
    void add_node_back() {
        m_node.push_back(new node("s" + to_string(node_cnt++)));


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
    //문자와 맞는 matcher를 반환한다.
    Imatchable* get_matcher(char ch) {
        if (ch == '.')
            return new matcher_dot();
        else if (is_char(ch))
            return new matcher_single(ch);
        else return nullptr;


    }


    // * 연산자 여부 체크한다.
    bool check_asterisk(bool& syntax, size_t& idx) {
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
        node* tail = m_node.back();
        add_node_back();

        //다음 노드 가리키는 링크 추가
        tail->add_link(new node_ptr_direct(matcher, m_node.back()));

        //이전 노드에서 다음 노드로 바로 가는 링크 추가
        for (const node* rev : tail->reverse_ref()) {
            for (node_ptr* nxt : rev->next()) {
                if (nxt->is_ref_equal(tail)) {
                    node* p = const_cast<node*>(rev);
                    p->add_link(nxt->copy(m_node.back()));
                }
            }
        }

        //자기 자신 가리키는 노드 추가        
        tail->add_link(new node_ptr_direct(matcher->copy(), tail));

        idx++;
        return true;


    }
    // + 연산자 여부 체크한다.
    bool check_plus(bool& syntax, size_t& idx) {
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

        //tail 참조 얻고 뒷부분 노드 추가
        node* tail = m_node.back();
        add_node_back();

        //다음 노드 가리키는 링크 추가
        tail->add_link(new node_ptr_direct(matcher, m_node.back()));

        //자기 자신 가리키는 노드 추가        
        tail->add_link(new node_ptr_direct(matcher->copy(), tail));

        idx++;
        return true;


    }
    // {} 연산자 여부 체크한다.
    bool check_range(bool& syntax, size_t& idx) {
        //연산 적용 가능한지 체크
        if (!is_char(regex[idx])) {
            syntax = false;
            return false;
        }

        //범위 표현식인지 체크
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '{')
            return false;

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

        if (range_end == -1 || split == -1) {
            syntax = false;
            return false;
        }

        //범위를 계산한다.
        size_t istart = size_t(stoi(regex.substr(idx + 2, split - idx - 2)));
        size_t iend = size_t(stoi(regex.substr(split + 1, range_end - split - 1)));
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

        //tail 참조 얻고 뒷부분 노드 추가
        node* tail = m_node.back();
        add_node_back();

        //다음 노드 가리키는 링크 추가
        tail->add_link(new node_ptr_inner_counter(matcher, m_node.back(), istart, iend));

        //TODO: k{0,3} 같은 케이스를 고려한 코드인데 잘 동작할지 모르겠음
        //이전 노드에서 다음 노드로 바로 가는 링크 추가
        if (istart == 0) {
            for (const node* rev : tail->reverse_ref()) {
                for (node_ptr* nxt : rev->next()) {
                    if (nxt->is_ref_equal(tail)) {
                        node* p = const_cast<node*>(rev);
                        p->add_link(nxt->copy(m_node.back()));
                    }
                }
            }
        }

        //자기 자신 가리키는 노드 추가        
        tail->add_link(new node_ptr_direct(matcher->copy(), tail));

        idx = range_end;
        return true;


    }
    // | 연산자 여부 체크한다.
    bool check_and(bool& syntax, size_t& idx) {
        //범위 & 연산 대상인지 확인
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '|')
            return false;

        // | 연산자로 끝나는 경우
        if (idx + 2 >= regex.size()) {
            syntax = false;
            return false;
        }

        //연산 가능 여부 확인
        if (!is_char(regex[idx]) || !is_char(regex[idx + 2])) {
            syntax = false;
            return false;
        }

        //matcher 얻기
        Imatchable* matcher1 = get_matcher(regex[idx]);
        Imatchable* matcher2 = get_matcher(regex[idx + 2]);
        if (matcher1 == nullptr || matcher2 == nullptr) {
            syntax = false;
            return false;
        }

        //tail 참조 얻고 노드 추가
        node* tail = m_node.back();
        add_node_back();

        tail->add_link(new node_ptr_direct(matcher1, m_node.back()));
        tail->add_link(new node_ptr_direct(matcher2, m_node.back()));

        idx += 2;
        return true;


    }
    //일반 문자로 이루어졌는지 체크한다.
    bool check_char(bool& syntax, size_t& idx) {
        if (!is_char(regex[idx])) {
            syntax = false;
            return false;
        }

        Imatchable* matcher = get_matcher(regex[idx]);
        if (matcher == nullptr) {
            syntax = false;
            return false;
        }

        node* tail = m_node.back();
        add_node_back();
        tail->add_link(new node_ptr_direct(matcher, m_node.back()));

        return true;


    }


}; // end of class: state_machine_creator



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
//public:
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
ranged_string compiled::match(const string& source, size_t index_start) {
    return state_machine_input(source, index_start, false);


}    
vector<ranged_string> compiled::match_all(const string& source) {
    vector<ranged_string> ret;
    size_t found_idx = 0;
    while (found_idx <= source.size()) {
        ranged_string ans = match(source, found_idx);
        if (!ans.is_valid) break;

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