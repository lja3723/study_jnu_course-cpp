#include <iostream>
using namespace std;



////#1: The first argument must be 'size_t'
//void* operator new(size_t sz) {
//	cout << "operator new" << endl;
//	return malloc(sz);
//}
//
////#2: overloading
//void* operator new(size_t sz, const char* s, int n) {
//	cout << "operator new2: " << s << " " << endl;
//	return malloc(sz);
//}
//
//void operator delete(void* p) {
//	cout << "operator delete" << endl;
//	free(p);
//}


namespace CH10 {
#define FN static void

	class AAA {
	private:
		char id;

	public:
		AAA() : id('z') {
			cout << "I am AAA!" << endl;
		}
		AAA(char c) {
			cout << "I am a " << c << "! " << endl;
			this->id = c;
		}
		~AAA() {
			cout << id << " will be back!" << endl;
		}

	};

	class klass {
	private:
		int a = 0x41424344; //for memory check
	public:
		klass() { cout << "Constructor!" << endl; }
		~klass() { cout << "Destructor!" << endl; }
		void foo() { cout << "Foooo!" << endl; }
	};

	template <class T>
	class smartptr {
	private:
		T* ptr;
	public:
		smartptr(T* p = 0) : ptr(p) {}
		~smartptr() {
			cout << "free the memory" << endl;
			delete ptr;
		}
		//dereference
		T& operator*() { return *ptr; }

		//for pointer of class or struct
		T* operator->() { return ptr; }
	};

	class member {
	private:
		int id;
	public:
		member(int _id): id(_id) {}
		void print() {
			cout << id << endl;
		}
	};
	
	struct node {
		int data;
		node* next;

		node(int d, node* n) : data(d), next(n) {}
	};

	class linkedlist_inline {
	private:
		node* head;
		node* current;
		int index;

	public:
		linkedlist_inline() : head(nullptr), current(nullptr), index(0) {}
		
		//약간 역전된 리스트 느낌쓰
		void push_front(int d) { head = new node(d, head); } //한 줄에 끝냈다!!!
		
		linkedlist_inline& next() {
			if (!current) current = head;
			else current = current->next;
			return *this;
		}
		int get() {
			return current->data;
		}

		int operator[] (int i) {
			for (node* s = head; false; i++) {
				;
			}
		}
	};


	//TODO: 에러 나는 이유 찾기
	class Point {
	private:
		int x, y;
		char* name;
	public:
		Point(int _x, int _y, const char* name = nullptr) : x(_x), y(_y) {
			this->name = new char[strlen(name)];
			strcpy(this->name, name);
			cout << "Constructor " << name << ": " << x << " " << y << endl;
		}
		~Point() { 
			cout << "Bye " << name << ": " << x << " " << y << endl; 
			delete[] name;
		}

		Point(const Point& p) : x(p.x), y(p.y), name(p.name) /*얕은 복사!!*/ {
			this->name = new char[strlen(p.name)];
			strcpy(this->name, p.name);	//깊은 복사!
			cout << "Copy Constructor " << name << ": " << x << " " << y << endl;
		}
	};



	class RunExample {
	public:
		FN dynamic_alloc() {
			int* p = new int;
			*p = 1024;
			delete p;

			int a = 1024;
			//delete& a; 정적 메모리 삭제는 불가능

			int arr_size;
			cout << "배열 사이즈: " << endl;
			cin >> arr_size;
			int* arr = new int[arr_size];

			for (int i = 0; i < arr_size; i++) arr[i] = i;
			for (int i = 0; i < arr_size; i++) cout << arr[i] << " ";

			delete[] arr;
		}
		FN class_ex() {

			//파괴자 호출 O
			{
				AAA a('a');
				AAA z;
			}
			cout << endl;

			//파괴자 호출 X
			//메모리가 누수된다!!
			{
				AAA* ptr1 = new AAA;
				AAA* ptr2 = new AAA('c');
			}
		}
		FN malloc_new_difference() {
			// malloc()/free() -> function
			//malloc 쓰면 객체 생성자가 호출되지 X
			{
				cout << "malloc()/free()" << endl;
				AAA* p1 = (AAA*)malloc(sizeof(AAA)); //생성자 호출 X
				free(p1); //파괴자 호출 X
			}
			cout << endl;

			// new/delete -> operator
			{
				cout << "new/delete" << endl;
				AAA* p1 = new AAA(); //생성자 호출
				delete p1; //파괴자 호출
				//그리고 '연산자' -> 연산자 오버로딩 가능!			
			}
			cout << endl;



		}
		FN new_del_oper_overload() /*새 개념*/  {
			int* p = new int(5);
			int* p2 = new(sizeof(2), "new new new", 2) int;

			delete p;
		}
		FN placement_new() /*새 개념*/ {

			{
				char reserve[1024] = "Hello!";
				cout << reserve << endl;

				//placement new
				//메모리를 같은 공간에서 더 크게 할당할 필요가 있을 때 사용
				klass* ptr = new (reserve) klass;

				cout << (void*)reserve << endl;
				cout << ptr << endl;

				cout << reserve << endl; //0x41424344(ABCD)가 overwrite됨
				
			}
			cout << "\n\n\n";
			
			//생성자를 여러번 호출하는 효과가 있음
			klass k;
			//k.klass(); 생성자 재호출 불가능
			k.~klass(); //파괴자는 여러번 호출 가능
			k.~klass();
			k.~klass();
			k.~klass();

			new (&k) klass; //생성자 호출 (여러 번 가능)
			new (&k) klass;
			new (&k) klass;
			new (&k) klass;
		}
		FN my_smartptr_ex() {
			{
				smartptr<int> ptr = new int;
				//int* ptr = new int;
				*ptr = 10;
				cout << "ptr is " << *ptr << endl;
			} cout << "\n\n";

			{
				smartptr<double> ptr = new double;
				*ptr = 3.14f;
				cout << "ptr is " << *ptr << endl;
			} cout << "\n\n";

			{
				smartptr<member> ptr = new member(10);
				ptr->print();
			}

		}
		FN lnline_list_ex() {
			linkedlist_inline s;
			s.push_front(10);
			s.push_front(20);
			s.push_front(30);
			s.push_front(40);

			cout << s.get() << endl;
			for (int i = 0; i < 3; i++) {
				cout << s.next().get() << endl;
			}
		}
		FN explicit_implicit_call() {
			Point p1(10, 20, "p1");
			Point p2 = Point(30, 40, "p2");
			Point p3 = p1; //생성자 호출 X (복사 생성자가 호출됨)
		}




	};


}

//int main() { CH10::RunExample::explicit_implicit_call(); }