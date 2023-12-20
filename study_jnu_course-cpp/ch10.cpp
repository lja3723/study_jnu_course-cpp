#include <iostream>
using namespace std;

////#1: The first argument must be 'size_t'
//void* operator new(size_t sz) {
//	cout << "operator new: sz = " << sz << endl;
//	return malloc(sz);
//}
//
////#2: overloading
//void* operator new(size_t sz, const char* s, int n) {
//	cout << "operator new2: sz = " << sz << ", <s,n> = <" << s << ", " << n << ">\n";
//	return malloc(sz);
//}
//
////이 선언은 이미 placement new가 사용해서 안됨
////void* operator new(size_t sz, void* t) {
////	cout << "operator new(sz_t, void*)" << endl;
////	return malloc(sz);
////}
//
//void operator delete(void* p) {
//	cout << "operator delete" << endl;
//	free(p);
//}
//
//void* operator new[](size_t sz) {
//	cout << "operator new[]" << endl;
//	return malloc(sz);
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
		void my_destructor_for_smart_ptr() { cout << "Foooo!" << endl; }
	};

	template <class T>
	class smartptr {
	private:
		T* ptr;
	public:
		smartptr(T* p = 0) : ptr(p) {}
		~smartptr() {
			cout << "free the memory" << endl;
			//delete ptr;
		}

		//conversion constructor(형변환 생성자)
		smartptr(long* p = 0) : ptr((T*)p) {}

		//conversion operator(형변환 연산자)
		operator T* () {
			return ptr;
		}

		//dereference
		T& operator*() { return *ptr; }

		//for pointer of class or struct
		T* operator->() { return ptr; }
	};
	void my_destructor_for_smart_ptr(int* p) {
		cout << "delete p!" << endl;
		delete p;
	}

	class member {
	private:
		int id;
	public:
		member(int _id) : id(_id) {}
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

	//TODO: 에러 나는 이유 찾기 -> solve
	//문자열 배열 동적할당할때 \0을 고려 안한게 원인이었음~~
	class Point {
	private:
		int x, y;
		char* name;
	public:
		Point(int _x, int _y, const char* name = nullptr) : x(_x), y(_y) {
			this->name = new char[strlen(name) + 1];
			memcpy(this->name, name, sizeof(char) * (strlen(name) + 1));
			cout << "Constructor " << name << ": " << x << " " << y << endl;
		}
		~Point() {
			cout << "Bye " << name << ": " << x << " " << y << endl;
			delete[] name;
		}

		Point(const Point& p) : x(p.x), y(p.y), name(p.name) /*얕은 복사!!*/ {
			this->name = new char[strlen(p.name) + 1];
			strcpy_s(this->name, sizeof(char) * (strlen(p.name) + 1), p.name);	//깊은 복사!
			cout << "Copy Constructor " << name << ": " << x << " " << y << endl;
		}
	};

	//member pointer ex
	class MyClass {
	public:
		int mem;
		int* pmem;
		MyClass(int m = 0) : mem(m), pmem(nullptr) {}
		void func(int k = 0) { cout << "mem func(k):" << k << endl; }
	};

	class parent { 
	public: 
		int x; 
		void pfun() { cout << "pfun()" << endl; }
	};
	class child : public parent { public: 
		int y;
		void pfun() { cout << "overrided pfun()" << endl; }
		void cfun() { cout << "cfun()" << endl; }
	};


	//my nullptr example
	const class {
	public:
		template <class T>
		operator T* () const {
			return 0;
		}

		//멤버 함수를 가리키는 포인터에 대한 변환
		template<class c, class t>
		operator t c::* () const {
			return 0;
		}

		void operator&() const = delete;
	} my_nullptr = {};

	void nptr_ex_func(int n) {
		cout << "void nptr_ex_func(int) called" << endl;
	}

	void nptr_ex_func(char* p) {
		cout << "void nptr_ex_func(char*) called" << endl;
	}
}

namespace CH10 {
	struct Klass { int a; };

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
			int* p = new int;
			//int* p2 = new("stringinput", 2) int;

			new double[5];
			new string;

			new(p) int;

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
		FN shared_ptr_ex() {
			shared_ptr<int> p1(new int);

			*p1 = 10;
			cout << *p1 << endl;
			cout << p1.use_count() << endl;
			cout << endl;

			shared_ptr<int> p2 = p1;
			cout << *p2 << endl;
			cout << p2.use_count() << endl;
			cout << endl;

			shared_ptr<int> p3(new int, my_destructor_for_smart_ptr); //assign the 파괴자
			cout << p3.use_count() << endl;
		}
		FN unique_ptr_ex() {
			unique_ptr<int> p1(new int);
			*p1 = 10;

			unique_ptr<int> p2 = std::move(p1);
			cout << *p2 << endl;
			//cout << *p1 << endl;
		}
		FN type_convert_ex() {
			long i = 1234;
			int j = i;

			int* p = (int*)&i;
			cout << *p << endl;

			smartptr<int> p1(new int);
			*p1 = 10;

			//다른 객체 -> 대상객체 형변환
			p1 = &i;
			cout << *p1 << endl;

			//대상객체 -> 다른 객체 형변환
			int* ptr = p1;
			cout << *ptr << endl;
		}
		FN type_casting_ex() {
			//c++에서는 캐스팅을 위한 연산자가 4개나 있다
			//생긴건 전부 템플릿처럼 생겼지만 "연산자"이다
			//	static_cast<>		-> 상식적인 변환만 허용함
			//	reinterpret_cast<>	-> C 스타일 캐스팅(메모리 공간 재해석)
			//	const_cast<>		-> 객체의 상수성 제거
			//	dynamic_cast<>		-> 동적으로 형변환 가능할 때 허용 //RTTI에서 다룸

			double pi = 3.14;
			int* ptr = (int*)(&pi); //이유가 없지만
			cout << *ptr << endl; //작동한다.

			//int* qtr = static_cast<int*>(&pi); 컴파일 타임 동작

			//메모리 재해석(메모리 범위를 초과하지만 않는다면 프로그래머 마음!)
			ptr = reinterpret_cast<int*>(&pi); 
			cout << *ptr << endl;

		}
		FN type_casting_ex2() {
			int i;
			const char str[] = { 'a', 'b', 'c' };
			const char* p_cstr = &str[1];
			//str[1] = 'B'; unable; because const!
			char* p_str = const_cast<char*>(p_cstr);
			*p_str = 'Z';
			for (i = 0; i < 3; i++) cout << str[i] << " "; //a Z c 출력
			cout << endl; 

			const int ARR[] = { 1, 2, 3 };
			const int* p_cARR = &ARR[1];
			int* p_ARR = const_cast<int*>(p_cARR);
			//ARR[1] = -200; unable; because const!
			*p_ARR = -200;
			for (i=0; i<3; i++) cout << ARR[i] << " "; //1 -200 3 출력
			cout << endl;

			const int INT = 1;
			const int* p_cINT = &INT;
			int* hack = const_cast<int*>(p_cINT);
			*hack = -100;
			cout << INT << endl; //1 출력
		}
		FN type_casting_ex3() {
			parent* p = new parent;
			child* c = new child;
			c->x = 1;
			c->y = 2;
			parent *p2 = static_cast<parent*>(c);
			p2 = new parent;
			p2->x = 10;
			child* c2 = static_cast<child*>(p2);

			cout << p2->x << endl;
			p2->pfun();
			cout << c2->x << "," << c2->y << endl;
			c2->pfun();
			c2->cfun();

		}
		FN nullptr_ex() {
			char str[] = { 'H', 'e', 'l', 'l', 'o', NULL };
			int* ptr = NULL;
			cout << ptr << "\n\n";

			nptr_ex_func(0);
			nptr_ex_func(NULL); //ambiguous
			nptr_ex_func(nullptr);

			//nullptr은 클래스에요! 포인터 형변환 호용하면서 다른것은 막은 상수 클래스
			nptr_ex_func(my_nullptr);
		}
		FN mem_ptr_ex() /*좀더 알아봐야할거같아요*/ {
			void (MyClass:: * mptr)(int);
			mptr = my_nullptr; // 이렇게 멤버함수를 가리키는 포인터로 변환이 가능하다.
			mptr = &MyClass::func;

			MyClass c;
			int MyClass::* pmem;
			pmem = &MyClass::mem;

			int* MyClass::* ppmem;
			ppmem = &MyClass::pmem;

			c.*pmem = 5;
			(c.*mptr)(3);
		}
		FN mem_ptr_ex2() {
			Klass k;
			Klass* qtr = &k;

			int Klass::* ptr;
			ptr = &Klass::a;

			k.*ptr = 123;
			cout << k.a << endl;

			qtr->*ptr = 234;
			cout << k.a << endl;
		}


	};


}

//int main() { CH10::RunExample::mem_ptr_ex2(); }