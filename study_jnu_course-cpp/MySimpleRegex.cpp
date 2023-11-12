#include "MySimpleRegex.h"
#include <map>

namespace assignment1 { 
using matched = MySimpleRegex::matched;
using compiled = MySimpleRegex::compiled;


/*******************   MySimpleRegex static �Լ�   *******************/
compiled MySimpleRegex::compile(const string& m_regex, int test = 0) {
    return compiled(m_regex, test);
}
matched MySimpleRegex::match(const string& m_regex, const string& source) {
    return compiled(m_regex).match(source);
}
vector<matched> MySimpleRegex::match_all(const string m_regex, const string& source) {
    return compiled(m_regex).match_all(source);
}



/*******************   ���� ��ġ Ŭ����   *******************/
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



/*******************   node �� �Ļ� Ŭ����   *******************/
//ó������ Ȱ��ȭ�� ���¿��� �� ù �ε����� ��ȯ�Ѵ�.
//is_accepted�� �� �νĵ� ���ڿ��� ù ��ġ�� �ǹ��Ѵ�.
unsigned compiled::node::index_start() const { return m_state_istart; }

//�� ��尡 ����Ű�� ��带 �߰��Ѵ�.
void compiled::node::addNode(node_ptr* _next) {
    next.push_back(_next);
    _next->link_reverse_ref(this);
}

//����ũ�� �߰��Ѵ�.
void compiled::node::add_link_reverse_ref(node* ref) {
    reverse_ref.push_back(ref);
}

//��带 Ȱ��ȭ ���·� �����.
void compiled::node::active(int state_istart) {
    m_state_istart = state_istart;
    m_state_active = true;
}

//��� ���¿� ���õ� ���� �÷��׵��� ��� Ŭ�����Ѵ�.
void compiled::node::clear_flags() {
    m_state_istart = 0;
    m_state_active = false;

    for (node_ptr* p : next)
        p->clear_flag();
}

//�� ��忡�� accept �Ǿ����� Ȯ���Ѵ�.
bool compiled::node::is_accepted() const {
    return m_is_terminal && m_state_active;
}

//��忡 �� ���ڸ� �Է���: ��尡 Ȱ��ȭ���� �� ���ڰ� ��Ī�Ǹ� ���� ���鿡�� Ȱ��ȭ�� ��û��
//��� Ȱ��ȭ & ���� ��Ī�� �� �͹̳� ����� ��� is_accepted�� true�� ��
void compiled::node::input(vector<active_request_info>& next_active, const char ch) {
    if (!m_state_active) return;

    for (int i = 0; i < next.size(); i++) {
        next[i]->active_request(next_active, m_state_istart, ch);
    }

    //�νĵǾ��� ��� true
    //m_isAccepted = m_state_active && match && m_isTerminal;
    m_state_active = false; // deactivate this node after matching
}



//node_ptr�� ��� matcher�� ��ȯ�Ѵ�.
compiled::node_ptr::~node_ptr() { if (matcher != nullptr) delete matcher; }

//����� ��忡 active�� ��û�Ѵ�. �ѹ��� �� ���� �ְ�, �׷��� ���� ���� ����
void compiled::node_ptr::active_request(vector<active_request_info>& next_active, unsigned state_istart, char ch) {
    //�䱸�Ǵ� ���ڿ� ��ġ�Ǵ� ��쿡�� �׼��� ����
    if (matcher->test(ch)) transition_action(next_active, state_istart);
}



/*******************   compiled Ŭ���� �Լ�   *******************/
//TODO: �־��� ����ǥ�������� ���¸ӽ� �����ϱ� �ʿ�
    //���±�� ��� �����̳ʿ� ���±�踦 �����Ѵ�.
void compiled::create_state_machine(int test) {

    //mock implementation
    //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
    if (test == 1) {
        // abc|ade
        m_node.resize(6/*����ǥ���� �ؼ� �� ��� ������ �Ǿ�� ��*/);

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

        // ���Ƿ� ��ȣ �޴� ��� ����
        m_get_epsilon.push_back(m_node[0]);

        // �͹̳� ��� ����
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
        //���ο� ���������!!
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

//������ ���±�踦 �����Ѵ�.
void compiled::delete_state_machine() {
    for (int i = 0; i < m_node.size(); i++)
        if (m_node[i] != nullptr)
            delete m_node[i];
}

//TODO: T* , T+ ���� ���̽����� �׸����ϰ� �����ϰ� �ٲ�� ��
//���±�迡 ���ڿ��� �Է����� �ް�, ���� ó������ accept�� ��ġ ������ ����Ѵ�.
matched compiled::state_machine_input(const string& src, unsigned index_start, bool check_at_front_only) {
    map<int, matched*> found;
    vector<node*> actives;
    vector<active_request_info> next_actives;

    ///*debug*/cout << "test for \"" << source << "\"" << endl;
    for (unsigned i = index_start; i < src.length(); i++) {
        ///*debug*/cout << "  >> input " << src[i] << endl;
        //���Ƿ� ��ȣ�� �� �ֱ�⸶�� �ο���
        //check_at_front_only�� ���ڿ� ������ ���ϰ� ��ġ�ϴ� ��츸 Ȯ���Ѵ�.
        //�� ���� 1ȸ�� ���Ƿ� ��ȣ�� �ο��Ѵ�.
        if (!check_at_front_only || i == 0) {
            for (node*& target : m_get_epsilon) {
                target->active(i);
                actives.push_back(target);
            }
        }

        // give ch all nodes
        // nv[j]�� active ���¿��ٸ� �� ���� �� nv[j]���� ������ ȭ��ǥ��
        // ������ ������ �� ����Ű�� ����� transited�� Ȱ��ȭ�Ѵ�.
        while (!actives.empty()) {
            node* target = actives.back();
            target->input(next_actives, src[i]);
            actives.pop_back();
        }

        //���� Ȱ��ȭ�� ��� ����Ʈ�� �޾ƿ��� Ȱ��ȭ�Ѵ�.
        while (!next_actives.empty()) {
            active_request_info& info = next_actives.back();
            (info.target)->active(info.start_index);
            actives.push_back(info.target);
            next_actives.pop_back();
        }

        //�͹̳� ��� ��ȸ �� accepted�Ǿ����� matched�� �����Ѵ�.
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
    //src�� ��� input�� �� ��� ����� �÷��� �ʱ�ȭ
    for (node* target : m_node) target->clear_flags();


    if (found.empty()) //��ġ�ϴ� ���� ����
        return matched::invalid();
    else {
        matched ret = *found.begin()->second;
        for (auto& p : found)
            delete p.second;

        return ret;
    }
}


} //end of namespace assignment1