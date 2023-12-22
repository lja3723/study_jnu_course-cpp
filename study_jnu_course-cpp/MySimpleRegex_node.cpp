#include "MySimpleRegex_node.h"
#include <string>
#include <algorithm>

namespace assignment1 {


compiled::node::node(string name, bool isTerminal) :
    m_name(name), m_is_terminal(isTerminal), m_next(0) {
    m_state_istart = 0;
    m_state_active = false;


}
compiled::node::~node() {
    for (int i = 0; i < m_next.size(); i++)
        if (m_next[i] != nullptr)
            //new로 할당받은 메모리를 반환한다.
            delete m_next[i]; 


}


/**************  조회 함수  **************/
const string& compiled::node::name() const {
    return m_name; 


}
size_t compiled::node::index_start() const {
    return m_state_istart; 


}
bool compiled::node::is_accepted() const {
    return m_is_terminal && m_state_active;


}
const vector<compiled::node_ptr*>& compiled::node::next() const {
    return m_next;


}
const vector<compiled::node_ptr*>& compiled::node::reverse_ref() const {
    return m_reverse_ref;


}



/**************  변경 함수  **************/
void compiled::node::add_link(node_ptr* _next) {
    if (_next == nullptr) return;

    //_next가 이미 등록되어 있으면 등록 X
    if (m_next.end() != find_if(m_next.begin(), m_next.end(),
        [_next](node_ptr* elem) { return elem->equals(_next); } )) {
        delete _next;
        return;
    }

    m_next.push_back(_next);
    _next->m_target->m_reverse_ref.push_back(_next);


}
void compiled::node::delete_link(node* _target, char _ch) {
    _target->m_reverse_ref.erase(remove_if(
        _target->m_reverse_ref.begin(), _target->m_reverse_ref.end(),
        [this, _ch](node_ptr* to_delete) {
            bool pred = to_delete->origin() == this;
            if (_ch != '\0') pred &= to_delete->test(_ch);
            return pred;
        }),
        _target->m_reverse_ref.end());

    m_next.erase(remove_if(
        m_next.begin(), m_next.end(),
        [_target, _ch](node_ptr* to_delete) { 
            bool pred = to_delete->is_ref_equal(_target);
            if (_ch != '\0') pred &= to_delete->test(_ch);
            if (pred) delete to_delete;
            return pred; 
        }),
        m_next.end());


}
void compiled::node::active(size_t state_istart) {
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

    // deactivate this node after matching
    m_state_active = false; 


}


} //end of namespace assignment1