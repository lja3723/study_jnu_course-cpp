#include <iostream>
using namespace std;

namespace cpp12_1{
//1. mystrcat
int mystrlen(const char* str) {
	int ret = -1;
	while (str[++ret] != '\0');
	return ret;
}

char* mystrcat(const char* str1, const char* str2) {
	int len1 = mystrlen(str1), len2 = mystrlen(str2);
	int len = len1 + len2 + 1;
	char* ret = new char[len];

	for (int i = 0; i < len1; i++)
		ret[i] = str1[i];
	for (int i = 0; i < len2; i++)
		ret[len1 + i] = str2[i];
	ret[len - 1] = '\0';

	return ret;
}

int main() {
	char* str = mystrcat("Hello!", "World!");
	cout << str << endl;
	delete[] str;
	str = nullptr;

	return 0;
}
/*************end*************/}
namespace cpp12_2{
//2. Reverse linked list
template <typename VALUE>
struct list {
	VALUE value;
	list* next;
};

template <typename VALUE>
list<VALUE>* append(list<VALUE>* tail, VALUE value) {
	list<VALUE>* node = new list<VALUE>;
	node->value = value;
	node->next = nullptr;
	if (tail != nullptr)
		tail->next = node;
	return node;
}

template <typename VALUE>
list<VALUE>* append(VALUE value) {
	return append<VALUE>(nullptr, value);
}

//reverse�� ����Ʈ�� �������κ��� �����ϸ� �����Ѵ�.
//����� ����Ʈ�� ��ȯ�ȴ�.
template <typename VALUE>
list<VALUE>* reverse_copy(list<VALUE>* head) {
	if (head == nullptr) return head;
	list<VALUE>* cur = head, * r_head = nullptr;

	while (cur != nullptr) {
		list<VALUE>* copied = new list<VALUE>;
		copied->value = cur->value;
		copied->next = r_head;
		r_head = copied;

		cur = cur->next;
	}

	return r_head;
}

//reverse�� ����Ʈ�� ���� ��ü�� �����ϸ� �����Ѵ�.
//ȣ�� �� ���� ���� �ݵ�� �ڱ� �ڽſ��� ����־�� �Ѵ�.
template <typename VALUE>
list<VALUE>* reverse_self(list<VALUE>* head) {
	if (head == nullptr) return nullptr;

	list<VALUE>* reversed = nullptr;

	while (head != nullptr) {
		list<VALUE>* cur = head;
		head = head->next;
		cur->next = reversed;
		reversed = cur;
	}

	return reversed;
}

template <typename VALUE>
void delete_list(list<VALUE>* head) {
	if (head == nullptr) return;
	while (head != nullptr) {
		list<VALUE>* next = head->next;
		delete head;
		head = next;
	}
}

int main() {
	list<int>* head;
	list<int>* loop;
	list<int>* tail;

	list<int>* r_head;

	tail = head = append(10);
	tail = append(tail, 20);
	tail = append(tail, 35);
	tail = append(tail, 45);
	tail = append(tail, 60);

	for (loop = head; loop != nullptr; loop = loop->next) {
		cout << loop->value << endl;
	}

	r_head = reverse_copy(head);
	cout << "Reversed (copied)" << endl;
	for (loop = r_head; loop != nullptr; loop = loop->next) {
		cout << loop->value << endl;
	}

	r_head = reverse_self(r_head);
	cout << "Re-Reversed (self)" << endl;
	for (loop = r_head; loop != nullptr; loop = loop->next) {
		cout << loop->value << endl;
	}

	delete_list(head);
	delete_list(r_head);

	return 0;
}
/*************end*************/}

int main() { cpp12_2::main(); }

#include <random>
#include <cstring>
namespace cpp12_3_1{
//3. Heap (data structure)
//included headers
template <typename V>
struct MyHeap {
	//���� ����
	int _capacity;	//���� �޸� ���� �뷮
	int _size;		//���� ���� ����
	V** _data;		//�� �����Ҵ� �迭
	int (*_pred_cmp)(V* a, V* b);	//�� ���� �Լ�������

	//�� �������̽�(�Լ� ������)
	void (*push)(V value);	//���� value�� push
	V(*pop)(void);			//���� ��Ʈ ��带 pop
	V(*top)(void);			//���� ��Ʈ ��带 view 
	bool (*empty)(void);	//���� ����ִ� �� ���� Ȯ��
};

//���� ������ �� ��ü ������ ����
template <typename V>
MyHeap<V>* _heap_this(MyHeap<V>* _this = nullptr, bool clear = false) {
	static MyHeap<V>* _this_hold = nullptr;
	if (clear) return _this_hold = nullptr;
	return (_this_hold = (_this == nullptr ? _this_hold : _this));
}

//���Ҹ� ������
template <typename V>
void _myheap_swap(V** a, V** b) {
	V* tmp = *a;
	*a = *b;
	*b = tmp;
}

//��ȿ���� ���� ���� ��ȯ�ؾ� �� ��� ȣ����
template <typename V>
V _myheap_invalid_value() {
	cout << "���� ������ϴ�! ��ȯ���� ��ȿ���� �ʽ��ϴ�." << endl;
	V tmp;
	memset(&tmp, 0, sizeof(V));
	return tmp; //0���� �ʱ�ȭ�� V ���� ���� ��ȯ
}

//�� push ����ü
template <typename V>
void _myheap_push(V value) {
	MyHeap<V>* _this = _heap_this<V>(); //���� ��ü ���� ���

	//���⵵ ���Ҹ��� ��������
	int& cap = _this->_capacity;
	int& size = _this->_size;
	V**& data = _this->_data;

	//�� �迭�� ���� ���� �� �� ū ���� �Ҵ�
	if (cap - 1 <= size) {
		V** new_data = new V*[2 * cap];
		for (int i = 0; i < cap; i++) {
			new_data[i] = data[i];
		}
		delete[] data;
		cap *= 2;
		data = new_data;
	}

	//�� �迭 ���� ���� ����
	int idx = ++size;
	data[idx] = new V;
	*data[idx] = value;

	//���Ե� ���Ҹ� ���ڸ��� ��ġ
	while (idx != 1) {
		if (_this->_pred_cmp(data[idx / 2], data[idx]) > 0) {
			_myheap_swap(&data[idx / 2], &data[idx]);
		}
		else break;
		idx /= 2;
	}
}

//�� pop ����ü
template <typename V>
V _myheap_pop() {
	MyHeap<V>* _this = _heap_this<V>(); //���� ��ü ���� ���

	//���� ������� ��� ó��
	if (_this->empty()) return _myheap_invalid_value<V>();

	//�� Ʈ�� �ֻ��� ��尡 ��ȯ�ǰ�, ���� ��尡 �ֻ��� ���� �ö��
	V ret = *(_this->_data[1]);
	delete _this->_data[1];
	_this->_data[1] = _this->_data[_this->_size];
	_this->_data[_this->_size] = nullptr;
	int size = --(_this->_size);
	int idx = 1;

	//�ֻ��� ��尡 ���ڸ��� ã�ư�
	while (true) {
		//���⵵ ���� ������ ���� ���� ����
		int l_idx = 2 * idx;
		int r_idx = l_idx + 1;
		int (*pred)(V*, V*) = _this->_pred_cmp;
		V** i_data = &_this->_data[idx];

		//����, ������ �ڽ� ��� ���� ������ ���
		if (l_idx <= size && r_idx <= size) {
			V** l_data = &_this->_data[l_idx]; //���⵵ ���ҿ� ����
			V** r_data = &_this->_data[r_idx]; //���⵵ ���ҿ� ����

			//����, ������ �ڽ� ��� ��ȯ ������ ���
			if (pred(*i_data, *r_data) > 0 && pred(*i_data, *l_data) > 0)
				//�� �ڽ� �� �� ���� �ڽİ� ��ȯ�ϱ�
				if (pred(*l_data, *r_data) > 0) {
					_myheap_swap(i_data, r_data);
					idx = r_idx;
				}
				else {
					_myheap_swap(i_data, l_data);
					idx = l_idx;
				}
			//���� �ڽĸ� ��ȯ ������ ��� ������ ��ȯ
			else if (pred(*i_data, *l_data) > 0) {
				_myheap_swap(i_data, l_data);
				idx = l_idx;
			}
			//������ �ڽĸ� ��ȯ �����ϸ� ������ ��ȯ			
			else if (pred(*i_data, *r_data) > 0) {
				_myheap_swap(i_data, r_data);
				idx = r_idx;
			}
			//�� �ڽ� ��� ��ȯ �������� ���� ���; ���ڸ��� ã�����Ƿ� while Ż��
			else break;
		}
		//���� �ڽĸ� ���� ������ ���
		else if (l_idx <= size) {
			V** l_data = &_this->_data[l_idx];
			if (pred(*i_data, *l_data) > 0) {
				_myheap_swap(i_data, l_data);
				idx = l_idx;
			}
			else break; //���ڸ��� ã��
		}
		//������ �ڽĸ� ���� ������ ���
		else if (r_idx <= size) {
			V** r_data = &_this->_data[r_idx];
			if (pred(*i_data, *r_data) > 0) {
				_myheap_swap(i_data, r_data);
				idx = r_idx;
			}
			else break; //���ڸ��� ã��
		}
		else break; //���ڸ��� ã��
	}

	return ret;
}

//�� top ����ü
template <typename V>
V _myheap_top() {
	MyHeap<V>* _this = _heap_this<V>();

	//���� ������� ��� ó��
	if (_this->empty()) return _myheap_invalid_value<V>();

	return *(_this->_data[1]);
}

//�� empty ����ü
template <typename V>
bool _myheap_empty() {
	MyHeap<V>* _this = _heap_this<V>();
	return _this->_size == 0;
}

//�� ��� �� ȣ���ؾ��ϴ� �Լ�
//pred_cmp: �� ���Ŀ� ���Ǵ� ���� �����Լ�
template <typename V>
MyHeap<V>* MyHeapInit(int (*pred_cmp)(V* a, V* b)) {
	MyHeap<V>* ret = new MyHeap<V>;

	//���� ���� �ʱ�ȭ
	ret->_capacity = 16;
	ret->_size = 0;
	ret->_data = new V*[ret->_capacity];
	ret->_pred_cmp = pred_cmp;

	//���� ������ �� ���� ����
	_heap_this(ret);

	//�� �������̽��� �� �Լ� ����
	ret->push = _myheap_push;
	ret->pop = _myheap_pop;
	ret->top = _myheap_top;
	ret->empty = _myheap_empty<V>;

	return ret;
}

//�� ��� �� �޸� ��ȯ
template <typename V>
void MyHeapDestroy(MyHeap<V>* heap) {
	if (heap == nullptr) return;

	if (heap->_data != nullptr) {
		for (int i = 0; i < heap->_size; i++) {
			if (heap->_data[i + 1] != nullptr)
				delete heap->_data[i + 1];
		}

		delete[] heap->_data;
	}

	delete heap;
	heap = nullptr;

	_heap_this<V>(nullptr, true);
}

//�� ���� �����Լ� (���� int)
int myheap_int_cmp(int* a, int* b) {
	return *a - *b;
}

//�� ���� �����Լ� (���� string)
int myheap_string_cmp(string* a, string* b) {
	return strcmp(a->c_str(), b->c_str());
}

int main() {
	/*******************************************/
	/*   CASE 1 : int�� ���� MyHeap ��� ����   */
	/*******************************************/
	cout << " [ CASE 1 ] \n\n";
	random_device rd;
	mt19937 gen(rd());
	//data_len ���� ��ŭ�� 1~data_len ���� ���� ���õ� ������ ����
	const int data_len = 195;
	int data_list[data_len] = { 0, };
	for (int i = 0; i < data_len; i++)
		data_list[i] = i + 1;
	for (int i = 0; i < data_len; i++) {
		int ridx = gen() % data_len;
		int tmp = data_list[ridx];
		data_list[ridx] = data_list[i];
		data_list[i] = tmp;
	}

	cout << "heap�� push�� ���Ҵ� ������� ������ �����ϴ�." << endl;
	int newline = 0; //����� ����
	int sep_num = 10; //�� �࿡ ��µ� ���� ��
	for (int i = 0; i < data_len; i++) {
		cout << data_list[i] << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << "\n\n";

	MyHeap<int>* heap = MyHeapInit<int>(myheap_int_cmp); //�� �ʱ�ȭ
	//���� ������ ������� ���� ����
	for (int i = 0; i < data_len; i++) {
		heap->push(data_list[i]);
	}

	cout << "heap���� pop�� ���Ҵ� ������� ������ �����ϴ�." << endl;
	//���� �� �� ���� ���Ҹ� pop�� �� ���
	newline = 0; //����� ����
	while (!heap->empty()) {
		cout << heap->pop() << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << endl;

	cout << "���� �����մϴ�." << endl;
	MyHeapDestroy(heap);

	/**********************************************/
	/*   CASE 2 : string�� ���� MyHeap ��� ����   */
	/**********************************************/
	cout << "\n\n\n [ CASE 2 ] \n\n";
	string str_data[24] = {
		"square", "method", "allocator", "string", "korean",
		"leftup", "cloud", "classification", "zebra", "symbol",
		"solution", "axes", "backend", "embedded", "nested",
		"signal", "korean", "samsung", "computer", "engineering",
		"semeconduct", "elephant", "denmark", "coefficient"
	};

	newline = 0;
	sep_num = 4;
	cout << "���� �� ����: " << endl;
	for (int i = 0; i < 24; i++) {
		cout << str_data[i] << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << "\n\n";

	MyHeap<string>* heap_str = MyHeapInit<string>(myheap_string_cmp); //�� �ʱ�ȭ
	//���� ������ ������� ���� ����
	for (int i = 0; i < 24; i++) {
		heap_str->push(str_data[i]);
	}

	newline = 0;
	cout << "���� �� ����: " << endl;
	while (!heap_str->empty()) {
		cout << heap_str->pop() << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << "\n\n";

	cout << "���� �����մϴ�." << endl;
	MyHeapDestroy(heap_str);

	return 0;
}
/*************end*************/}
namespace cpp12_3_2{
//3. Heap Implementation with Linked-List
//Ư���� ���ǵ� ��ũ�� ����Ʈ�� ���� ������
template <typename E>
class MyHeapListImpl {
private:
	struct _Node {
		E* elem;
		_Node* parent, * left, * right;
		_Node* prev, * next;
	};

	int _size;
	_Node* _head, * _tail;
	_Node* _leaf_perant; //push�Ǵ� �� �� ���� ����� �θ� ���
	bool _leaf_child; //���� ���� �� �θ��� � �ڽ����� ǥ��
	//_leaf_child�� false -> left, true -> right ��忡 ���Ե� �����̶�� �ǹ�

	int (*pred)(const E&, const E&); //�� ���� ����	

	void swap(E*& a, E*& b) {
		E* tmp = a;
		a = b;
		b = tmp;
	}

public:
	MyHeapListImpl(int (*pred)(const E&, const E&)) {
		_size = 0;
		_head = _tail = _leaf_perant = nullptr;
		_leaf_child = false;
		this->pred = pred;
	}
	void push(E elem) {
		_Node* node = new _Node;
		node->elem = new E;
		*(node->elem) = elem;
		_size++;

		// ù ���� ������ ���
		if (_head == nullptr) {
			_head = _tail = _leaf_perant = node;
			_leaf_child = false;

			node->next = node->prev = nullptr;
			node->parent = node->left = node->right = nullptr;
			return;
		}

		// �� ��带 tail�� ����
		node->prev = _tail;
		node->parent = _leaf_perant;
		node->next = node->left = node->right = nullptr;

		_tail->next = node;
		_tail = node;

		if (!_leaf_child) {
			_leaf_perant->left = node;
			_leaf_child = true;
		}
		else {
			_leaf_perant->right = node;
			_leaf_perant = _leaf_perant->next;
			_leaf_child = false;
		}

		//�� ��尡 ���ڸ��� ã�ư� ������ ��ȯ
		_Node* p = _tail;
		while (p->parent != nullptr && pred(*p->parent->elem, *p->elem) > 0) {
			swap(p->elem, p->parent->elem);
			p = p->parent;
		}
	}
	E pop() {
		if (empty()) {
			cout << "unable to pop(): empty Heap" << endl;
			return 0;
		}

		//���� ��带 ��忡 �ø�
		E ret = *(_head->elem);
		delete _head->elem;
		_head->elem = _tail->elem;
		_Node* delNode = _tail;
		if (_tail->prev != nullptr) {
			_tail->prev->next = nullptr;
		}
		_tail = _tail->prev;

		if (delNode->parent != nullptr) {
			if (!_leaf_child) {
				_leaf_perant = _leaf_perant->prev;
				_leaf_child = true;
				_leaf_perant->right = nullptr;
			}
			else {
				_leaf_child = false;
				_leaf_perant->left = nullptr;
			}
		}
		else {
			_leaf_perant = nullptr;
			_leaf_child = false;
			_head = nullptr;
		}
		delete delNode;
		_size--;

		//��Ʈ ��带 ���ڸ��� ã�ư�
		_Node* p = _head;
		while (p != nullptr) {
			if (p->left == nullptr && p->right == nullptr) break;
			if (p->left == nullptr) {
				if (pred(*p->elem, *p->right->elem) > 0) {
					swap(p->elem, p->right->elem);
					p = p->right;
				}
				else break;
			}
			else if (p->right == nullptr) {
				if (pred(*p->elem, *p->left->elem) > 0) {
					swap(p->elem, p->left->elem);
					p = p->left;
				}
				else break;
			}
			else {
				int left_cond = pred(*p->elem, *p->left->elem);
				int right_cond = pred(*p->elem, *p->right->elem);

				if (left_cond > 0 && right_cond > 0) {
					if (pred(*p->left->elem, *p->right->elem) > 0) {
						swap(p->elem, p->right->elem);
						p = p->right;
					}
					else {
						swap(p->elem, p->left->elem);
						p = p->left;
					}
				}
				else if (left_cond > 0) {
					swap(p->elem, p->left->elem);
					p = p->left;
				}
				else if (right_cond > 0) {
					swap(p->elem, p->right->elem);
					p = p->right;
				}
				else break;
			}
		}

		return ret;
	}
	bool empty() {
		return _size == 0;
	}
	int size() {
		return _size;
	}
	~MyHeapListImpl() {
		while (_head != nullptr) {
			_Node* delNode = _head;
			delete _head->next;
			_head = _head->next;
			delete delNode;
		}
	}
};

int myheaplistimpl_int_cmp(const int& a, const int& b) {
	return a - b;
}

int main() {
	int arr[] = { 4, 10, 15, 34, 3, 53, 21, 39, 40 };
	int len = sizeof(arr) / sizeof(*arr);

	MyHeapListImpl<int> heap = MyHeapListImpl<int>(myheaplistimpl_int_cmp);

	for (int i = 0; i < len; i++)
		heap.push(arr[i]);

	while (!heap.empty())
		cout << heap.pop() << " ";


	return 0;
}
/*************end*************/}