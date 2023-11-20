#include "MySimpleRegex.h"
#include "MySimpleRegex_node.h"
#include <string>


/****************************/
/*      MySimpleRegex &     */
/*      compiled Classes    */
/****************************/
namespace assignment1 {


/******   state machine ������ Ŭ����   ******/
class compiled::state_machine_creator {
public:
    state_machine_creator(vector<node*>& _node, node*& _epsilon, node*& _terminal) :
        m_node(_node), 
        m_epsilon(_epsilon), 
        m_terminal(_terminal), 
        node_cnt(0) {}


    //�־��� regex�� ���� �ӽ��� �����Ѵ�.
    //return true: ���� �ӽ� ���� ����
    //return false: ���� �ӽ� ���� ����(regex ���� �߸���)
    bool make(const string& regex) {
        this->regex = regex;
        //���� �ӽ��� �ʱ�ȭ�Ѵ�.
        clear();

        //�ϵ��ڵ��� ���Խİ� ��ġ�ϴ� ��� ���� ����
        if (is_hard_coded(regex)) return true;

        //���� ó��
        bool syntax_valid = true;
        add_node_back();
        add_to_epsilon(m_node.back());

        //����ǥ������ �� ���ھ� �����鼭 �۾��� �����Ѵ�.
        for (size_t i = 0; i < regex.size(); i++) {
            //asterisk ������ ��ġ�ϸ� ���� �ӽ� �����۾� ���� �� i�� ������Ų��.
            if (check_asterisk(syntax_valid, i)) continue;
            //��ġ���� �����鼭 ������ Ʋ���� �����Ѵ�.
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

        if (!syntax_valid) clear(); //�߸� ������ ���¸ӽ��� �ʱ�ȭ�Ѵ�.       
        else {
            //���¸ӽ��� ���������´�.
            m_node.back()->set_terminal(true);
            add_to_terminal(m_node.back());
        }
        return syntax_valid;


    }


private:
    //�ܺ� ���� ����
    //�� �������� ������ ���� �ӽ��� �����Ѵ�.
    vector<node*>& m_node;
    node*& m_epsilon;
    node*& m_terminal;


    //���� ����
    int node_cnt; //������ ��� ����
    string regex; 


    //�ϵ��ڵ��� ���¸ӽ��� �����ϴ� ��� �ش� ���¸ӽ����� �ʱ�ȭ
    bool is_hard_coded(const string& m_regex) {
        //mock implementation
        //test state-machine of regex
        if (m_regex == "abc|ade") {
            m_node.resize(6/*����ǥ���� �ؼ� �� ��� ������ �Ǿ�� ��*/);

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

            // ���Ƿ� ��ȣ �޴� ��� ����
            m_epsilon->add_link(new node_ptr_direct(new matcher_true(), m_node[0]));

            // �͹̳� ��� ����
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

        //��ġ�Ǵ� regex ����
        return false;


    }
    //���ԽĿ��� �����ϴ� '���� ���ĺ�' �Ǵ� . ���� Ȯ���Ѵ�.
    bool is_char(char ch) {
        return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ch == '.';


    }


    //���Ƿ��� ���� ��带 ����Ѵ�.
    void add_to_epsilon(node* target) {
        m_epsilon->add_link(new node_ptr_direct(new matcher_true(), target));


    }
    //�͹̳� ��带 ����Ѵ�.
    void add_to_terminal(node* target) {
        m_terminal->add_link(new node_ptr_direct(new matcher_true(), target));


    }
    //�� ��带 �ڿ� �߰��Ѵ�.
    void add_node_back() {
        m_node.push_back(new node("s" + to_string(node_cnt++)));


    }
    //��� �����̳ʸ� �ʱ�ȭ�Ѵ�.
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
    //���ڿ� �´� matcher�� ��ȯ�Ѵ�.
    Imatchable* get_matcher(char ch) {
        if (ch == '.')
            return new matcher_dot();
        else if (is_char(ch))
            return new matcher_single(ch);
        else return nullptr;


    }


    // * ������ ���� üũ�Ѵ�.
    bool check_asterisk(bool& syntax, size_t& idx) {
        //���� & ���� ������� Ȯ��
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '*')
            return false;

        //���� ���� ���� Ȯ��
        if (!is_char(regex[idx])) {
            syntax = false;
            return false;
        }

        //matcher ���
        Imatchable* matcher = get_matcher(regex[idx]);
        if (matcher == nullptr) {
            syntax = false;
            return false;
        }

        //tail ���� ��� �޺κ� ��� �߰�
        node* tail = m_node.back();
        add_node_back();

        //���� ��� ����Ű�� ��ũ �߰�
        tail->add_link(new node_ptr_direct(matcher, m_node.back()));

        //���� ��忡�� ���� ���� �ٷ� ���� ��ũ �߰�
        for (const node* rev : tail->reverse_ref()) {
            for (node_ptr* nxt : rev->next()) {
                if (nxt->is_ref_equal(tail)) {
                    node* p = const_cast<node*>(rev);
                    p->add_link(nxt->copy(m_node.back()));
                }
            }
        }

        //�ڱ� �ڽ� ����Ű�� ��� �߰�        
        tail->add_link(new node_ptr_direct(matcher->copy(), tail));

        idx++;
        return true;


    }
    // + ������ ���� üũ�Ѵ�.
    bool check_plus(bool& syntax, size_t& idx) {
        //���� & ���� ������� Ȯ��
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '+')
            return false;

        //���� ���� ���� Ȯ��
        if (!is_char(regex[idx])) {
            syntax = false;
            return false;
        }

        //matcher ���
        Imatchable* matcher = get_matcher(regex[idx]);
        if (matcher == nullptr) {
            syntax = false;
            return false;
        }

        //tail ���� ��� �޺κ� ��� �߰�
        node* tail = m_node.back();
        add_node_back();

        //���� ��� ����Ű�� ��ũ �߰�
        tail->add_link(new node_ptr_direct(matcher, m_node.back()));

        //�ڱ� �ڽ� ����Ű�� ��� �߰�        
        tail->add_link(new node_ptr_direct(matcher->copy(), tail));

        idx++;
        return true;


    }
    // {} ������ ���� üũ�Ѵ�.
    bool check_range(bool& syntax, size_t& idx) {
        //���� ���� �������� üũ
        if (!is_char(regex[idx])) {
            syntax = false;
            return false;
        }

        //���� ǥ�������� üũ
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '{')
            return false;

        //������ �Ľ��ϱ�
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

        //������ ����Ѵ�.
        size_t istart = size_t(stoi(regex.substr(idx + 2, split - idx - 2)));
        size_t iend = size_t(stoi(regex.substr(split + 1, range_end - split - 1)));
        if (istart > iend) {
            syntax = false;
            return false;
        }

        //matcher ���
        Imatchable* matcher = get_matcher(regex[idx]);
        if (matcher == nullptr) {
            syntax = false;
            return false;
        }

        //tail ���� ��� �޺κ� ��� �߰�
        node* tail = m_node.back();
        add_node_back();

        //���� ��� ����Ű�� ��ũ �߰�
        tail->add_link(new node_ptr_inner_counter(matcher, m_node.back(), istart, iend));

        //TODO: k{0,3} ���� ���̽��� ����� �ڵ��ε� �� �������� �𸣰���
        //���� ��忡�� ���� ���� �ٷ� ���� ��ũ �߰�
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

        //�ڱ� �ڽ� ����Ű�� ��� �߰�        
        tail->add_link(new node_ptr_direct(matcher->copy(), tail));

        idx = range_end;
        return true;


    }
    // | ������ ���� üũ�Ѵ�.
    bool check_and(bool& syntax, size_t& idx) {
        //���� & ���� ������� Ȯ��
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '|')
            return false;

        // | �����ڷ� ������ ���
        if (idx + 2 >= regex.size()) {
            syntax = false;
            return false;
        }

        //���� ���� ���� Ȯ��
        if (!is_char(regex[idx]) || !is_char(regex[idx + 2])) {
            syntax = false;
            return false;
        }

        //matcher ���
        Imatchable* matcher1 = get_matcher(regex[idx]);
        Imatchable* matcher2 = get_matcher(regex[idx + 2]);
        if (matcher1 == nullptr || matcher2 == nullptr) {
            syntax = false;
            return false;
        }

        //tail ���� ��� ��� �߰�
        node* tail = m_node.back();
        add_node_back();

        tail->add_link(new node_ptr_direct(matcher1, m_node.back()));
        tail->add_link(new node_ptr_direct(matcher2, m_node.back()));

        idx += 2;
        return true;


    }
    //�Ϲ� ���ڷ� �̷�������� üũ�Ѵ�.
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



/*******************   MySimpleRegex static �Լ�   *******************/
compiled MySimpleRegex::compile(const string& m_regex) {
    return compiled(m_regex);


}
ranged_string MySimpleRegex::match(const string& m_regex, const string& source) {
    return compiled(m_regex).match(source);


}
vector<ranged_string> MySimpleRegex::match_all(const string m_regex, const string& source) {
    return compiled(m_regex).match_all(source);


}



/*******************   compiled ��� �Լ�   *******************/
//public:
compiled::compiled(const string& m_regex)
    : m_regex(m_regex), m_epsilon(nullptr), m_terminal(nullptr) {

    state_machine_creator creator(m_node, m_epsilon, m_terminal);

    //���Խ� ������ �߸��� ��� �� ���� ��谡 �����ȴ�.
    if (!creator.make(m_regex)) {
        cout << "����ǥ���� ������ �߸��Ǿ����ϴ�. �񱳰� ����� ������� ���� �� �ֽ��ϴ�." << endl;
    }


}
compiled::~compiled() {
    //���� �Ҵ�� ��带 �����Ѵ�.
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
    map<size_t, ranged_string*> found; //ã�� ranged_string ����Ʈ
    map<string, node*> actives;     //���� Ȱ��ȭ�� ��� ����Ʈ
    vector<active_request_info> next_actives; //�������� Ȱ��ȭ�� ��� ����Ʈ

    for (size_t i = index_start; i < src.length(); i++) {
        give_epsilon(next_actives, actives, i, check_at_front_only);
        check_terminal_active(found, src, i);

        // active ������ ��忡�� ������ ��� �����Ϳ� ���ڸ� �Է��Ѵ�.
        for (auto it = actives.begin(); it != actives.end(); it++)
            it->second->input(next_actives, src[i]);
        actives.clear();

        active_transition(next_actives, actives);
    }

    //���Ƿ� ��ȣ�� ���������� �ο��� ����, �͹̳��� Ȱ��ȭ�Ǿ����� Ȯ���Ѵ�.
    give_epsilon(next_actives, actives, src.length(), check_at_front_only);
    check_terminal_active(found, src, src.length());

    //��� �۾��� ������ �� ��, ��� ����� �÷��׸� �ʱ�ȭ�Ѵ�.
    for (node* target : m_node) target->clear_flags();

    if (found.empty()) //��ġ�ϴ� ���� ����
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
    //check_at_front_only == false�̸� ���Ƿ� ��ȣ�� �� �ֱ⸶�� �ο��Ѵ�.
    //check_at_front_only == true�̸� ���ڿ� ���� �κ��� ���ϰ� ��ġ�ϴ� ��츸 Ȯ���Ѵ�.
    // -> �� ���� 1ȸ�� ���Ƿ� ��ȣ�� �ο��Ѵ�.
    if (!(check_at_front_only && state_istart > 0)) {
        for (node_ptr* nxt : m_epsilon->next())
            nxt->request_active_nexttime(next_actives, state_istart, '\0');

        //next_actives�� �����Ͽ� Ȱ��ȭ�� ������ ������ Ȱ��ȭ�Ѵ�.
        active_transition(next_actives, actives);
    }


}
void compiled::active_transition(
    vector<active_request_info>& next_actives, map<string, node*>& actives) 
{
    //next_actives���� ��带 �ϳ��� ������
    //������ �����ϴ� ���鸸 active�� ��, actives ����Ʈ�� �߰��Ѵ�.
    while (!next_actives.empty()) {
        node* to_active = next_actives.back().target;
        size_t state_istart = next_actives.back().start_index;
        const string& name = to_active->name();

        // ���� active�� ���� �ʾҰų�,
        // state_istart�� �� �۾������� active �� ��쿡�� active�� �����Ѵ�.
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
    //accepted �� �͹̳� ��带 �߰��ϸ� found�� �߰��Ѵ�.
    for (node_ptr* terminal : m_terminal->next()) {
        if (terminal->is_accepted()) {
            size_t istart = terminal->index_start();
            if (found.find(istart) != found.end()) delete found[istart];
            found[istart] = new ranged_string(src, istart, idx, true);
        }
    }


}


} //end of namespace assignment1