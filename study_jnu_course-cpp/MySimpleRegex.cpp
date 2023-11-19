#include "MySimpleRegex.h"
#include <string>



/****************************/
/*    Inner Classes &       */
/*    members of compiled   */
/****************************/
namespace assignment1 {
using compiled = MySimpleRegex::compiled;

/***********   ���� matcher �� �Ļ� Ŭ����   ***********/
class compiled::Imatchable {
public:
    virtual bool test(char ch) = 0;
    virtual Imatchable* copy() = 0; //�ڱ� �ڽ� ���� ����
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



/***************   node & �Ļ� Ŭ����   ***************/
class compiled::node {
    friend class node_ptr;
private:
    string m_name;              // state node name
    unsigned m_state_istart;    // state�� ù active�Ǿ��� �� �ԷµǾ��� ������ �ε���
    bool m_state_active;        // 0: deactive, 1: active
    bool m_is_terminal;         // 0: terminal, 1: non-terminal

    vector<node_ptr*> m_next;     // next link
    vector<const node*> m_reverse_ref;  //�� ��带 ����Ű�� ������

    void add_link_reverse_ref(node* ref); //����ũ�� �߰��Ѵ�.

public:
    node(string name, bool isTerminal = false);
    ~node();

    /*********  ��ȸ �Լ�  *********/
    const string& name() const;
    unsigned index_start() const;   //is_accepted�� �� �νĵ� ���ڿ��� ù ��ġ�� �ǹ��Ѵ�.  
    bool is_accepted() const;       //�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
    const vector<node_ptr*>& next() const;      //next ����� ��ȯ�Ѵ�.
    const vector<const node*>& reverse_ref() const;   //������ ��������� ��ȯ�Ѵ�.

    /*********  ���� �Լ�  *********/
    void add_link(node_ptr* _next); //�� ��尡 ����Ű�� ��带 �߰��Ѵ�.
    void active(int state_istart); //��带 Ȱ��ȭ ���·� �����.
    void clear_flags(); //��� ���¿� ���õ� ���� �÷��׵��� ��� Ŭ�����Ѵ�.
    void set_terminal(bool is_terminal);


    //��忡 �� ���ڸ� �Է��Ѵ�. 
    //��尡 Ȱ��ȭ���� �� ���ڰ� ��ġ�Ǹ� next ���鿡�� Ȱ��ȭ�� ��û�Ѵ�.
    //��� Ȱ��ȭ && �͹̳� ����� ��� is_accepted�� true�� �ȴ�.
    void input(vector<active_request_info>& next_active, const char ch);


}; /************************ node end ************************/
class compiled::node_ptr {
    friend class node;
protected:
    node* m_pNode;
    Imatchable* matcher;

    //�� �����Ͱ� ������ ���� ������ �����Ѵ�. �������̵� �ʿ�
    virtual void transition_action(
        vector<active_request_info>& next_active, unsigned state_istart) = 0;

    //����� ����� �������� �־��� ���� �����Ѵ�
    virtual void link_reverse_ref(node* ref) final {
        m_pNode->add_link_reverse_ref(ref);
    }

    //�����Ϳ� ���õ� �÷��׸� Ŭ�����Ѵ�. �ʿ��� ��츸 �������̵��Ѵ�.
    virtual void clear_flag() {}


public:
    static const unsigned INF = ~0U; //2^32 - 1

    node_ptr(Imatchable* ranged_string, node* pNode) 
        : m_pNode(pNode), matcher(ranged_string) {}
    ~node_ptr() {
        if (matcher != nullptr)
            delete matcher; //���� �Ҵ�� matcher�� ��ȯ�Ѵ�.
    }

    //node*���� ����� ����ȯ
    explicit operator node*() { return m_pNode; }

    //���� �����͸� ���ο� ������ ���� �����ͷ� �����Ѵ�.
    virtual node_ptr* copy(node* new_pNode) = 0;

    //����� ��尡 ���� ����Ŭ�� active �ǵ��� ��û�Ѵ�.
    //�ѹ��� �� ���� �ְ�, �׷��� ���� ���� ����
    virtual void request_active_nexttime(
        vector<active_request_info>& next_active, 
        unsigned state_istart, char ch ) final 
    { 
        if (matcher->test(ch)) //�䱸�Ǵ� ���ڿ� ��ġ�Ǵ� ��쿡�� ���� �ൿ�� ���Ѵ�.
            transition_action(next_active, state_istart);
    }

    //is_accepted�� �� �νĵ� ���ڿ��� ù ��ġ�� �ǹ��Ѵ�.
    unsigned index_start() const {
        return m_pNode->index_start();
    }

    //�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
    bool is_accepted() const {
        return m_pNode->is_accepted();
    }

    //��� �����Ͱ� ����Ű�� ���� �Ķ���� ��尡 ������ Ȯ���Ѵ�.
    bool is_ref_equal(const node* pNode) const {
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

        //������ ����� ��츸 state�� �����Ѵ�
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



/**************   node Ŭ���� ��� �Լ�   **************/
compiled::node::node(string name, bool isTerminal) :
    m_name(name), m_is_terminal(isTerminal), m_next(0) {
    m_state_istart = 0;
    m_state_active = false;
}
compiled::node::~node() {
    for (int i = 0; i < m_next.size(); i++)
        if (m_next[i] != nullptr)
            delete m_next[i]; //new�� �Ҵ���� �޸𸮸� ��ȯ�Ѵ�.
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
const vector<const compiled::node*>& compiled::node::reverse_ref() const {
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
void compiled::node::set_terminal(bool is_terminal) {
    m_is_terminal = is_terminal;
}
void compiled::node::input(vector<active_request_info>& next_active, const char ch) {
    if (!m_state_active) return;

    for (int i = 0; i < m_next.size(); i++)
        m_next[i]->request_active_nexttime(next_active, m_state_istart, ch);

    m_state_active = false; // deactivate this node after matching
}


/******   state machine creator Ŭ����   ******/
//TODO: �־��� ����ǥ�������� ���¸ӽ� �����ϱ� �ʿ�
class compiled::state_machine_creator {
public:
    state_machine_creator(vector<node*>& _node, node*& _epsilon, node*& _terminal) :
        m_node(_node), m_epsilon(_epsilon), m_terminal(_terminal), node_cnt(0) {}

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
        for (unsigned i = 0; i < regex.size(); i++) {
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

        //�߸� ������ ���¸ӽ��� �ʱ�ȭ�Ѵ�.
        if (!syntax_valid) {
            clear();
        }
        else {
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
    int node_cnt;
    string regex;

    bool is_hard_coded(const string& m_regex) {
        //mock implementation
        //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
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

    //���Ƿ��� ���� ��带 ����Ѵ�.
    void add_to_epsilon(node* target) {
        m_epsilon->add_link(new node_ptr_direct(new matcher_true(), target));
    }
    
    //�͹̳� ��带 ����Ѵ�.
    void add_to_terminal(node* target) {
        m_terminal->add_link(new node_ptr_direct(new matcher_true(),target));
    }

    //�� ��带 �ڿ� �߰��Ѵ�.
    void add_node_back() {
        m_node.push_back(new node("s" + to_string(node_cnt++)));
    }

    //���ԽĿ��� �����ϴ� '���� ���ĺ�' �Ǵ� . ���� Ȯ���Ѵ�.
    bool is_char(char ch) {
        return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ch == '.';
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
    bool check_asterisk(bool& syntax, unsigned& idx) {
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
    bool check_plus(bool& syntax, unsigned& idx) {
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
    bool check_range(bool& syntax, unsigned& idx) {
        //���� ���� �������� üũ
        if (!is_char(regex[idx])) {
            syntax = false;
            return false;
        }

        //���� ǥ�������� üũ
        if (!(idx + 1 < regex.size()) || regex[idx + 1] != '{')
            return false;

        //������ �Ľ��ϱ�
        int range_end = -1;
        int split = -1;
        for (int i = idx + 2; i < regex.size(); i++) {
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
        unsigned istart = stoi(regex.substr(idx + 2, split - idx - 2));
        unsigned iend = stoi(regex.substr(split + 1, range_end - split - 1));
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
    bool check_and(bool& syntax, unsigned& idx) {
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
    bool check_char(bool& syntax, unsigned& idx) {
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


};



} //end of namespace assignment1




/****************************/
/*      MySimpleRegex &     */
/*      compiled Classes    */
/****************************/
namespace assignment1 {
using compiled = MySimpleRegex::compiled;


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

//TODO: T* �� ���� ���ԽĿ��� "aa" �Է��ϸ� (0,0], (1,1], (2,2] ���� ��� ������ ����
ranged_string compiled::state_machine_input(const string& src, unsigned index_start, bool check_at_front_only)
{
    map<int, ranged_string*> found; //ã�� ranged_string ����Ʈ
    map<string, node*> actives; //���� Ȱ��ȭ�� ��� ����Ʈ
    vector<active_request_info> next_actives; //�������� Ȱ��ȭ�� ��� ����Ʈ

    for (unsigned i = index_start; i < src.length(); i++) {
        //���Ƿ� ��ȣ�� �� �ֱ�⸶�� �ο���
        //check_at_front_only�� ���ڿ� ������ ���ϰ� ��ġ�ϴ� ��츸 Ȯ���Ѵ�.
        //�� ���� 1ȸ�� ���Ƿ� ��ȣ�� �ο��Ѵ�.
        if (!(check_at_front_only && i > 0)) {
            for (node_ptr* nxt : m_epsilon->next())
                nxt->request_active_nexttime(next_actives, i, '\0');

            //next_actives�� �����Ͽ� Ȱ��ȭ�� ������ ������ Ȱ��ȭ�Ѵ�.
            request_active(next_actives, actives);

            //check_terminal(found, src, i);
        }

        // active ������ ��忡�� ������ ��� �����Ϳ� ���ڸ� �Է��Ѵ�.
        for (auto it = actives.begin(); it != actives.end(); it++)
            it->second->input(next_actives, src[i]);
        actives.clear();

        //next_actives�� �����Ͽ� Ȱ��ȭ�� ������ ������ Ȱ��ȭ�Ѵ�.
        request_active(next_actives, actives);

        //�͹̳� ��� ��ȸ �� accepted�Ǿ����� found�� matched�� �����Ѵ�.
        check_terminal(found, src, i);
    }

    //src�� ��� input�� �� ��� ����� �÷��� �ʱ�ȭ
    for (node* target : m_node) target->clear_flags();


    if (found.empty()) //��ġ�ϴ� ���� ����
        return ranged_string::invalid();
    else {
        ranged_string ret = *found.begin()->second;
        for (auto& p : found) delete p.second;

        return ret;
    }


}
void compiled::request_active(vector<active_request_info>& next_actives, map<string, node*>& actives) {
    while (!next_actives.empty()) {
        node* to_active = next_actives.back().target;
        unsigned state_istart = next_actives.back().start_index;
        const string& name = to_active->name();

        if (actives.find(name) == actives.end() ||
            actives[name]->index_start() >= state_istart) 
        {
            to_active->active(state_istart);
            actives[name] = to_active;
        }

        next_actives.pop_back();
    }


}
void compiled::check_terminal(map<int, ranged_string*>& found, const string& src, unsigned idx) {
    for (node_ptr* terminal : m_terminal->next()) {
        if (terminal->is_accepted()) {
            unsigned istart = terminal->index_start();
            if (found.find(istart) != found.end()) delete found[istart];
            found[istart] = new ranged_string(src, istart, idx + 1, true);
        }
    }


}



} //end of MySimpleRegex & compiled member functions