#include <iostream>
using namespace std;

//1. mystrcat
int cpp12_mystrlen(const char* str) {
	int ret = -1;
	while (str[++ret] != '\0');
	return ret;
}

char* cpp12_mystrcat(const char* str1, const char* str2) {
	int len1 = cpp12_mystrlen(str1), len2 = cpp12_mystrlen(str2);
	int len = len1 + len2 + 1;
	char* ret = new char[len];

	for (int i = 0; i < len1; i++)
		ret[i] = str1[i];
	for (int i = 0; i < len2; i++)
		ret[len1 + i] = str2[i];
	ret[len - 1] = '\0';

	return ret;
}

int cpp12_main1() {
	char* str = cpp12_mystrcat("Hello!", "World!");
	cout << str << endl;
	delete[cpp12_mystrlen(str) + 1] str;
	str = nullptr;

	return 0;
}



//2. Reverse linked list
template <typename VALUE>
struct cpp12_list {
	VALUE value;
	cpp12_list* next;
};

template <typename VALUE>
cpp12_list<VALUE>* append(cpp12_list<VALUE>* tail, VALUE value) {
	cpp12_list<VALUE>* node = new cpp12_list<VALUE>;
	node->value = value;
	node->next = nullptr;
	if (tail != nullptr)
		tail->next = node;
	return node;
}

template <typename VALUE>
cpp12_list<VALUE>* append(VALUE value) {
	return append<VALUE>(nullptr, value);
}

template <typename VALUE>
cpp12_list<VALUE>* reverse(cpp12_list<VALUE>* head) {
	if (head == nullptr) return head;
	cpp12_list<VALUE>* cur = head, *r_head = nullptr;

	while (cur != nullptr) {
		cpp12_list<VALUE>* copied = new cpp12_list<VALUE>;
		copied->value = cur->value;

		if (r_head == nullptr) 
			copied->next = nullptr;
		else 
			copied->next = r_head;
		r_head = copied;

		cur = cur->next;
	}

	return r_head;
}

template <typename VALUE>
void cpp12_delete_list(cpp12_list<VALUE>* head) {
	if (head == nullptr) return;
	while (head != nullptr) {
		cpp12_list<VALUE>* next = head->next;
		delete head;
		head = next;
	}
}

int cpp12_main2() {
	cpp12_list<int>* head;
	cpp12_list<int>* loop;
	cpp12_list<int>* tail;

	cpp12_list<int>* r_head;

	tail = head = append(10);
	tail = append(tail, 20);
	tail = append(tail, 30);
	tail = append(tail, 40);

	for (loop = head; loop != nullptr; loop = loop->next) {
		cout << loop->value << endl;
	}

	r_head = reverse(head);

	cout << "Reversed" << endl;
	for (loop = r_head; loop != nullptr; loop = loop->next) {
		cout << loop->value << endl;
	}

	cpp12_delete_list(head);
	cpp12_delete_list(r_head);

	return 0;
}

//3. Heap (data structure)
#include <random>
#include <cstring>
template <typename V>
struct cpp12_MyHeap {
	//���� ����
	int _capacity;	//���� �޸� ���� �뷮
	int _size;		//���� ���� ����
	V* _data;		//�� �����Ҵ� �迭
	int (*_pred_cmp)(V* a, V* b);	//�� ���� �Լ�������

	//�� �������̽�(�Լ� ������)
	void (*push)(V value);	//���� value�� push
	V(*pop)(void);			//���� ��Ʈ ��带 pop
	V(*top)(void);			//���� ��Ʈ ��带 view 
	bool (*empty)(void);	//���� ����ִ� �� ���� Ȯ��
};

//���� ������ �� ��ü ������ ����
template <typename V>
cpp12_MyHeap<V>* _heap_this(cpp12_MyHeap<V>* _this = nullptr, bool clear = false) {
	static cpp12_MyHeap<V>* _this_hold = nullptr;
	if (clear) return _this_hold = nullptr;
	return (_this_hold = (_this == nullptr ? _this_hold : _this));
}

//���Ҹ� ������
template <typename V>
void _myheap_swap(V* a, V* b) {
	V tmp = *a;
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
	cpp12_MyHeap<V>* _this = _heap_this<V>(); //���� ��ü ���� ���
	
	//���⵵ ���Ҹ��� ��������
	int& cap = _this->_capacity;
	int& size = _this->_size;
	V*& data = _this->_data;

	//�� �迭�� ���� ���� �� �� ū ���� �Ҵ�
	if (cap - 1 <= size) {
		V* new_data = new V[2 * cap];
		for (int i = 0; i < cap; i++) {
			new_data[i] = data[i];
		}
		delete[] data;
		cap *= 2;
		data = new_data;
	}

	//�� �迭 ���� ���� ����
	int idx = ++size;
	data[idx] = value;

	//���Ե� ���Ҹ� ���ڸ��� ��ġ
	while (idx != 1) {		
		if (_this->_pred_cmp(&data[idx / 2], &data[idx]) > 0) {
			_myheap_swap(&data[idx / 2], &data[idx]);
		}
		else break;
		idx /= 2;
	}
}

//�� pop ����ü
template <typename V>
V _myheap_pop() {
	cpp12_MyHeap<V>* _this = _heap_this<V>(); //���� ��ü ���� ���

	//���� ������� ��� ó��
	if (_this->empty()) return _myheap_invalid_value<V>();
	
	//�� Ʈ�� �ֻ��� ��尡 ��ȯ�ǰ�, ���� ��尡 �ֻ��� ���� �ö��
	V ret = _this->_data[1];
	_this->_data[1] = _this->_data[_this->_size--];
	int size = _this->_size;
	int idx = 1;

	//�ֻ��� ��尡 ���ڸ��� ã�ư�
	while (true) {
		//���⵵ ���� ������ ���� ���� ����
		int l_idx = 2 * idx;
		int r_idx = l_idx + 1;
		int (*pred)(V*, V*) = _this->_pred_cmp;
		V* i_data = &_this->_data[idx];

		//����, ������ �ڽ� ��� ���� ������ ���
		if (l_idx <= size && r_idx <= size) {
			V* l_data = &_this->_data[l_idx]; //���⵵ ���ҿ� ����
			V* r_data = &_this->_data[r_idx]; //���⵵ ���ҿ� ����

			//����, ������ �ڽ� ��� ��ȯ ������ ���
			if (pred(i_data, r_data) > 0 && pred(i_data, l_data) > 0)
				//�� �ڽ� �� �� ���� �ڽİ� ��ȯ�ϱ�
				if (pred(l_data, r_data) > 0) {
					_myheap_swap(i_data, r_data);
					idx = r_idx;
				}
				else {
					_myheap_swap(i_data, l_data);
					idx = l_idx;
				}
			//���� �ڽĸ� ��ȯ ������ ��� ������ ��ȯ
			else if (pred(i_data, l_data) > 0) {
				_myheap_swap(i_data, l_data);
				idx = l_idx;
			}
			//������ �ڽĸ� ��ȯ �����ϸ� ������ ��ȯ			
			else if (pred(i_data, r_data) > 0) {
				_myheap_swap(i_data, r_data);
				idx = r_idx;
			}
			//�� �ڽ� ��� ��ȯ �������� ���� ���; ���ڸ��� ã�����Ƿ� while Ż��
			else break;
		}
		//���� �ڽĸ� ���� ������ ���
		else if (l_idx <= size) {
			V* l_data = &_this->_data[l_idx];
			if (pred(i_data, l_data) > 0) {
				_myheap_swap(i_data, l_data);
				idx = l_idx;
			}
			else break; //���ڸ��� ã��
		}
		//������ �ڽĸ� ���� ������ ���
		else if (r_idx <= size) {
			V* r_data = &_this->_data[r_idx];
			if (pred(i_data, r_data) > 0) {
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
	cpp12_MyHeap<V>* _this = _heap_this<V>();
	
	//���� ������� ��� ó��
	if (_this->empty()) return _myheap_invalid_value<V>();

	return _this->_data[1];
}

//�� empty ����ü
template <typename V>
bool _myheap_empty() {
	cpp12_MyHeap<V>* _this = _heap_this<V>();
	return _this->_size == 0;
}

//�� ��� �� ȣ���ؾ��ϴ� �Լ�
//pred_cmp: �� ���Ŀ� ���Ǵ� ���� �����Լ�
template <typename V>
cpp12_MyHeap<V>* MyHeapInit(int (*pred_cmp)(V* a, V* b)) {
	cpp12_MyHeap<V>* ret = new cpp12_MyHeap<V>;

	//���� ���� �ʱ�ȭ
	ret->_capacity = 16;
	ret->_size = 0;
	ret->_data = new V[ret->_capacity];
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
void MyHeapDestroy(cpp12_MyHeap<V>* heap) {
	if (heap == nullptr) return;
	
	if (heap->_data != nullptr)
		delete[] heap->_data;

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

int cpp12_main3() {
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

	cpp12_MyHeap<int>* heap = MyHeapInit<int>(myheap_int_cmp); //�� �ʱ�ȭ
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

	cpp12_MyHeap<string>* heap_str = MyHeapInit<string>(myheap_string_cmp); //�� �ʱ�ȭ
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