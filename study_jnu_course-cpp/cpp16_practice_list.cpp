#include <iostream>
using namespace std;


namespace cpp16_1 {	
//1. Upgrade the Linked list class

template <typename E>
class my_list {
	struct _node {
	public:
		E elem;
		_node* next;
	};
	
	int m_length, m_current;
	_node* p_head, *p_tail, *p_current;

	void release() {
		while (p_head != nullptr) {
			_node* delNode = p_head;
			p_head = p_head->next;
			delete delNode;
		}
		m_length = m_current = 0;
		p_head = p_tail = p_current = nullptr;
	}

public:
	my_list() : 
		m_length(0), 
		m_current(0),
		p_head(nullptr),
		p_tail(nullptr),
		p_current(nullptr) {}

	~my_list() {
		release();
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

		m_current = 0;
		p_current = p_head;
	}
	void info() {
		cout << "length: " << m_length
			<< ", current: " << m_current << endl;
	}

	/********  new code here  ********/
	//복사 생성자 (깊은 복사)
	my_list(const my_list<E>& other) : my_list() {
		_node* h = other.p_head;

		while (h != nullptr) {
			append(h->elem);
			h = h->next;
		}

		while (m_current < other.m_current) next();
	}
	//대입 연산자 (기존 것 버리고 파라미터 깊은 복사)
	my_list& operator=(const my_list<E>& other) {
		release();
		my_list<E> tmp(other);
		p_head = tmp.p_head;
		p_tail = tmp.p_tail;
		p_current = tmp.p_current;
		m_length = tmp.m_length;
		m_current = tmp.m_current;
		tmp.p_head = nullptr;
		return *this;
	}

	my_list operator+(E elem) {
		return my_list<E>(*this).append(elem);
	}
	my_list operator+(const my_list<E>& other) {
		if (p_tail == nullptr) 
			return my_list<E>(other);

		my_list<E> first(*this), last(other);
		first.p_tail->next = last.p_head;
		first.p_tail = last.p_tail;
		last.p_head = nullptr;

		return first;
	}
	my_list operator-(int n) {
		my_list<E> ret(*this);

		_node* p = ret.p_head;
		while (n-- > 1) p = p->next;

		_node* del = p->next;
		p->next = del->next;
		delete del;

		return ret;
	}

	void printall() {
		_node* p = p_head;
		while (p != nullptr) {
			cout << "[" << p->elem << "]";
			p = p->next;
		}
		cout << endl;
	}
};

int main() {
	my_list<int> l;
	my_list<int> ll;

	l.append(10).append(20).append(30).append(40);

	l.next().next();
	cout << l.get() << endl; // print out 30

	ll = ll + 100 + 200 + 300; // append values 100, 200 and 300 to ll.

	ll.next();
	cout << ll.get() << endl; // print out 200

	ll = ll + l; // concatenate the list 'l' to the list 'll';
	// The list 'll' would be '[100][200][300][10][20][30][40]'
	//                   index:   0    1    2   3   4   5   6
	ll.printall();

	ll.next().next().next();
	cout << ll.get() << endl; // print out 20

	ll = ll - 5; // remove the 5-th node.
	ll.next();
	cout << ll.get() << endl; // print out 40, not 30. 
	ll.printall();
	
	return 0;
}


}

namespace cpp16_2 {	
//2. New linked list

template <typename T>
struct node {
	T data;
	node* next;
	int index;

	node(T d, node* n, int i) : data(d), next(n), index(i) {}
};

template <class T>
class slist {
private:
	node<T>* head;
	node<T>* current;
	int index; //다음에 push_front될 인덱스 값

public:
	slist() : head(0), current(0), index(0) {}
	void push_front(const T& d) { head = new node<T>(d, head, index++); }

	slist& next() {
		if (!current) current = head;
		else current = current->next;
		return *this;
	}
	int get() {
		return current->data;
	}

	T operator[](int i) {
		node<T>* s;
		for (s = head; s->index != i; s = s->next);
		return s->data;
	}


	/******  new code here  ******/
	void push_back(const T& d) {
		node<T>* s;
		if (head == nullptr) {
			head = new node<T>(d, nullptr, 0);
			return;
		}

		for (s = head; s->next != nullptr; s = s->next) s->index++;
		s->index++;
		s->next = new node<T>(d, nullptr, 0);
	}
};

int main() {
	slist<int> s;

	s.push_front(10);
	s.push_front(20);
	s.push_front(30);
	s.push_front(40);
	s.push_front(50);
	for (int i = 0; i < 5; i++)
		cout << s[i] << endl;
	cout << endl;

	s.push_back(-100);
	s.push_back(-200);
	s.push_back(-300);
	for (int i = 0; i < 8; i++)
		cout << s[i] << endl;
	cout << endl;

	slist<int> ss;
	ss.push_back(500);
	ss.push_back(300);
	ss.push_back(100);
	cout << ss[0] << endl;
	cout << ss[1] << endl;
	cout << ss[2] << endl;

	//cout << s.next().next().next().get() << endl;
	return 0;
}


}


//int main() { cpp16_2::main(); }
