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
	//내부 변수
	int _capacity;	//힙의 메모리 차지 용량
	int _size;		//힙의 원소 개수
	V* _data;		//힙 동적할당 배열
	int (*_pred_cmp)(V* a, V* b);	//힙 정렬 함수포인터

	//힙 인터페이스(함수 포인터)
	void (*push)(V value);	//힙에 value를 push
	V(*pop)(void);			//힙의 루트 노드를 pop
	V(*top)(void);			//힙의 루트 노드를 view 
	bool (*empty)(void);	//힙이 비어있는 지 여부 확인
};

//현재 생성된 힙 객체 참조값 저장
template <typename V>
cpp12_MyHeap<V>* _heap_this(cpp12_MyHeap<V>* _this = nullptr, bool clear = false) {
	static cpp12_MyHeap<V>* _this_hold = nullptr;
	if (clear) return _this_hold = nullptr;
	return (_this_hold = (_this == nullptr ? _this_hold : _this));
}

//원소를 스왑함
template <typename V>
void _myheap_swap(V* a, V* b) {
	V tmp = *a;
	*a = *b;
	*b = tmp;
}

//유효하지 않은 값을 반환해야 할 경우 호출함
template <typename V>
V _myheap_invalid_value() {
	cout << "힙이 비었습니다! 반환값은 유효하지 않습니다." << endl;
	V tmp;
	memset(&tmp, 0, sizeof(V));
	return tmp; //0으로 초기화된 V 형식 값을 반환
}

//힙 push 구현체
template <typename V>
void _myheap_push(V value) {
	cpp12_MyHeap<V>* _this = _heap_this<V>(); //현재 객체 참조 얻기
	
	//복잡도 감소목적 참조변수
	int& cap = _this->_capacity;
	int& size = _this->_size;
	V*& data = _this->_data;

	//힙 배열이 가득 차면 두 배 큰 공간 할당
	if (cap - 1 <= size) {
		V* new_data = new V[2 * cap];
		for (int i = 0; i < cap; i++) {
			new_data[i] = data[i];
		}
		delete[] data;
		cap *= 2;
		data = new_data;
	}

	//힙 배열 끝에 원소 삽입
	int idx = ++size;
	data[idx] = value;

	//삽입된 원소를 제자리에 위치
	while (idx != 1) {		
		if (_this->_pred_cmp(&data[idx / 2], &data[idx]) > 0) {
			_myheap_swap(&data[idx / 2], &data[idx]);
		}
		else break;
		idx /= 2;
	}
}

//힙 pop 구현체
template <typename V>
V _myheap_pop() {
	cpp12_MyHeap<V>* _this = _heap_this<V>(); //현재 겍체 참조 얻기

	//힙이 비어있을 경우 처리
	if (_this->empty()) return _myheap_invalid_value<V>();
	
	//힙 트리 최상위 노드가 반환되고, 말단 노드가 최상위 노드로 올라옴
	V ret = _this->_data[1];
	_this->_data[1] = _this->_data[_this->_size--];
	int size = _this->_size;
	int idx = 1;

	//최상위 노드가 제자리를 찾아감
	while (true) {
		//복잡도 감소 목적용 별명 변수 선언
		int l_idx = 2 * idx;
		int r_idx = l_idx + 1;
		int (*pred)(V*, V*) = _this->_pred_cmp;
		V* i_data = &_this->_data[idx];

		//왼쪽, 오른쪽 자식 모두 접근 가능한 경우
		if (l_idx <= size && r_idx <= size) {
			V* l_data = &_this->_data[l_idx]; //복잡도 감소용 별명
			V* r_data = &_this->_data[r_idx]; //복잡도 감소용 별명

			//왼쪽, 오른쪽 자식 모두 교환 가능한 경우
			if (pred(i_data, r_data) > 0 && pred(i_data, l_data) > 0)
				//두 자식 중 더 작은 자식과 교환하기
				if (pred(l_data, r_data) > 0) {
					_myheap_swap(i_data, r_data);
					idx = r_idx;
				}
				else {
					_myheap_swap(i_data, l_data);
					idx = l_idx;
				}
			//왼쪽 자식만 교환 가능한 경우 무조건 교환
			else if (pred(i_data, l_data) > 0) {
				_myheap_swap(i_data, l_data);
				idx = l_idx;
			}
			//오른쪽 자식만 교환 가능하면 무조건 교환			
			else if (pred(i_data, r_data) > 0) {
				_myheap_swap(i_data, r_data);
				idx = r_idx;
			}
			//두 자식 모두 교환 가능하지 않은 경우; 제자리를 찾았으므로 while 탈출
			else break;
		}
		//왼쪽 자식만 접근 가능한 경우
		else if (l_idx <= size) {
			V* l_data = &_this->_data[l_idx];
			if (pred(i_data, l_data) > 0) {
				_myheap_swap(i_data, l_data);
				idx = l_idx;
			}
			else break; //제자리를 찾음
		}
		//오른쪽 자식만 접근 가능한 경우
		else if (r_idx <= size) {
			V* r_data = &_this->_data[r_idx];
			if (pred(i_data, r_data) > 0) {
				_myheap_swap(i_data, r_data);
				idx = r_idx;
			}
			else break; //제자리를 찾음
		}
		else break; //제자리를 찾음
	}

	return ret;
}

//힙 top 구현체
template <typename V>
V _myheap_top() {
	cpp12_MyHeap<V>* _this = _heap_this<V>();
	
	//힙이 비어있을 경우 처리
	if (_this->empty()) return _myheap_invalid_value<V>();

	return _this->_data[1];
}

//힙 empty 구현체
template <typename V>
bool _myheap_empty() {
	cpp12_MyHeap<V>* _this = _heap_this<V>();
	return _this->_size == 0;
}

//힙 사용 전 호출해야하는 함수
//pred_cmp: 힙 정렬에 사용되는 정렬 조건함수
template <typename V>
cpp12_MyHeap<V>* MyHeapInit(int (*pred_cmp)(V* a, V* b)) {
	cpp12_MyHeap<V>* ret = new cpp12_MyHeap<V>;

	//내부 변수 초기화
	ret->_capacity = 16;
	ret->_size = 0;
	ret->_data = new V[ret->_capacity];
	ret->_pred_cmp = pred_cmp;

	//현재 생성된 힙 참조 연결
	_heap_this(ret);

	//힙 인터페이스와 실 함수 연결
	ret->push = _myheap_push;
	ret->pop = _myheap_pop;
	ret->top = _myheap_top;
	ret->empty = _myheap_empty<V>;

	return ret;
}

//힙 사용 후 메모리 반환
template <typename V>
void MyHeapDestroy(cpp12_MyHeap<V>* heap) {
	if (heap == nullptr) return;
	
	if (heap->_data != nullptr)
		delete[] heap->_data;

	delete heap;
	heap = nullptr;

	_heap_this<V>(nullptr, true);
}

//힙 정렬 조건함수 (원소 int)
int myheap_int_cmp(int* a, int* b) {
	return *a - *b;
}

//힙 정렬 조건함수 (원소 string)
int myheap_string_cmp(string* a, string* b) {
	return strcmp(a->c_str(), b->c_str());
}

int cpp12_main3() {
	/*******************************************/
	/*   CASE 1 : int에 대한 MyHeap 사용 예시   */
	/*******************************************/
	cout << " [ CASE 1 ] \n\n";
	random_device rd;
	mt19937 gen(rd());
	//data_len 길이 만큼의 1~data_len 값이 랜덤 셔플된 데이터 생성
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

	cout << "heap에 push될 원소는 순서대로 다음과 같습니다." << endl;
	int newline = 0; //개행용 변수
	int sep_num = 10; //한 행에 출력될 숫자 수
	for (int i = 0; i < data_len; i++) {
		cout << data_list[i] << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << "\n\n";

	cpp12_MyHeap<int>* heap = MyHeapInit<int>(myheap_int_cmp); //힙 초기화
	//힙에 데이터 순서대로 원소 삽입
	for (int i = 0; i < data_len; i++) {
		heap->push(data_list[i]);
	}

	cout << "heap에서 pop된 원소는 순서대로 다음과 같습니다." << endl;
	//힙이 빌 때 까지 원소를 pop한 후 출력
	newline = 0; //개행용 변수
	while (!heap->empty()) {
		cout << heap->pop() << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << endl;

	cout << "힙을 제거합니다." << endl;
	MyHeapDestroy(heap);

	/**********************************************/
	/*   CASE 2 : string에 대한 MyHeap 사용 예시   */
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
	cout << "정렬 전 원소: " << endl;
	for (int i = 0; i < 24; i++) {
		cout << str_data[i] << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << "\n\n";

	cpp12_MyHeap<string>* heap_str = MyHeapInit<string>(myheap_string_cmp); //힙 초기화
	//힙에 데이터 순서대로 원소 삽입
	for (int i = 0; i < 24; i++) {
		heap_str->push(str_data[i]);
	}
	
	newline = 0;
	cout << "정렬 후 원소: " << endl;
	while (!heap_str->empty()) {
		cout << heap_str->pop() << " ";
		if (++newline % sep_num == 0) cout << endl;
	}
	cout << "\n\n";

	cout << "힙을 제거합니다." << endl;
	MyHeapDestroy(heap_str);

	return 0;
}



//3. Heap Implementation with Linked-List
//특별히 정의된 링크드 리스트로 힙을 구현함
template <typename E>
class MyHeapListImpl {
	struct _Node {
		E elem;
		_Node* parent, *left, *right;
		_Node* prev, *next;
	};

	int _size;
	_Node* _head, *_tail;
	_Node* _leaf_perant; //push되는 그 때 말단 노드의 부모 노드
	bool _leaf_child; //말단 노드는 그 부모노드 어떤 자식인지 표시

	int (*pred)(const E&, const E&); //힙 정렬 조건
		//false -> left , true -> right

	void swap(E& a, E& b) {
		E tmp = a;
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
		node->elem = elem;

		// 첫 원소 삽입인 경우
		if (_head == nullptr) {
			_head = _tail = _leaf_perant = node;
			_leaf_child = false;

			node->next = node->prev = nullptr;
			node->parent = node->left = node->right = nullptr;
			return;
		}

		// 새 노드를 tail에 삽입
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

		//새 노드가 제자리를 찾아갈 때까지 교환
		_Node* p = _tail;
		while (p->parent != nullptr && pred(p->parent->elem, p->elem) > 0) {
			swap(p->elem, p->parent->elem);
		}		
	}
	E pop() {
		if (empty()) {
			cout << "unable to pop(): empty Heap" << endl;
			return 0;
		}

		//말단 노드를 헤드에 올림
		E ret = _head->elem;
		_head->elem = _tail->elem;
		_Node* delNode = _tail;
		delNode->prev->next = nullptr;
		_tail = _tail->prev;

		if (!_leaf_child) {
			_leaf_perant = _leaf_perant->prev;
			_leaf_child = true;
			_leaf_perant->right = nullptr;
		}
		else {
			_leaf_child = false;
			_leaf_perant->left = nullptr;
		}
		delete delNode;

		//루트 노드를 제자리에 찾아감
		_Node* p = _head;
		while (true) {
			if (p->left == nullptr && p->right == nullptr) break;
			if (p->left == nullptr) {
				if (pred(p->elem, p->right->elem) > 0) {
					swap(p->elem, p->right->elem);
					p = p->right;
				}
				else break;
			}
			else if (p->right == nullptr) {
				if (pred(p->elem, p->left->elem) > 0) {
					swap(p->elem, p->left->elem);
					p = p->left;
				}
				else break;
			}
			else {
				int left_cond = pred(p->elem, p->left->elem);
				int right_cond = pred(p->elem, p->right->elem);

				if (left_cond > 0 && right_cond > 0) {
					if (pred(p->left->elem, p->right->elem) > 0) {
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
			_head = _head->next;
			delete _head;
		}
	}
};

int myheaplistimpl_int_cmp(const int& a, const int& b) {
	return a - b;
}

void main3_2() {
	int arr[] = { 4, 10, 15, 34, 3, 53, 21, 39, 40 };
	int len = sizeof(arr) / sizeof(*arr);

	MyHeapListImpl<int> heap = MyHeapListImpl<int>(myheaplistimpl_int_cmp);

	for (int i = 0; i < len; i++)
		heap.push(arr[i]);

	while (!heap.empty())
		cout << heap.pop() << " ";

}

int main() {
	main3_2();
}