#include <iostream>
using namespace std;

namespace cpp15 {
//1. Linked list with classes
template <typename E>
class my_list {
	struct _node {
	public:
		E elem;
		_node* next;
	};
	
	int m_length, m_current;
	_node* p_head, *p_tail, *p_current;

public:
	my_list() : 
		m_length(0), 
		m_current(0),
		p_head(nullptr),
		p_tail(nullptr),
		p_current(nullptr) {}

	~my_list() {
		while (p_head != nullptr) {
			_node* delNode = p_head;
			p_head = p_head->next;
			delete delNode;
		}
	}

	my_list& append(E elem) {
		_node* newNode = new _node;
		newNode->elem = elem;
		newNode->next = nullptr;

		if (p_head == nullptr) {
			p_head = p_tail = p_current = newNode;
		}
		else {
			p_tail->next = newNode;
			p_tail = p_tail->next;
		}

		m_length++;
		return *this;
	}
	my_list& next() {
		if (m_length <= m_current + 1)
			throw std::out_of_range("unable to next(): 리스트 범위를 초과하였습니다.");

		m_current++;
		p_current = p_current->next;

		return *this;
	}
	E get() {
		if (p_current == nullptr)
			throw std::out_of_range("unable to get(): 리스트 범위를 초과하였습니다.");

		return p_current->elem;
	}
	void rewind() {
		if (p_head == nullptr)
			throw std::out_of_range("unable to rewind(): 리스트가 비어있습니다.");

		p_current = p_head;
	}
	void info() {
		cout << "length: " << m_length
			<< ", current: " << m_current << endl;
	}
};

int main() {
	my_list<int> l;

	l.append(10).append(20).append(30).append(40);
	l.next().next();
	cout << l.get() << endl; // print out 30

	l.info(); // print out 'length: 4, current: 2 (or 3 depending on your start index)'
	l.rewind();
	cout << l.get() << endl; // print out 10 
	return 0;
}
}