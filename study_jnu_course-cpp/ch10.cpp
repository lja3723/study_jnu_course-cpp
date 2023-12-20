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
////�� ������ �̹� placement new�� ����ؼ� �ȵ�
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

		//conversion constructor(����ȯ ������)
		smartptr(long* p = 0) : ptr((T*)p) {}

		//conversion operator(����ȯ ������)
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

		//�ణ ������ ����Ʈ ������
		void push_front(int d) { head = new node(d, head); } //�� �ٿ� ���´�!!!

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

	//TODO: ���� ���� ���� ã�� -> solve
	//���ڿ� �迭 �����Ҵ��Ҷ� \0�� ��� ���Ѱ� �����̾���~~
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

		Point(const Point& p) : x(p.x), y(p.y), name(p.name) /*���� ����!!*/ {
			this->name = new char[strlen(p.name) + 1];
			strcpy_s(this->name, sizeof(char) * (strlen(p.name) + 1), p.name);	//���� ����!
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

		//��� �Լ��� ����Ű�� �����Ϳ� ���� ��ȯ
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
			//delete& a; ���� �޸� ������ �Ұ���

			int arr_size;
			cout << "�迭 ������: " << endl;
			cin >> arr_size;
			int* arr = new int[arr_size];

			for (int i = 0; i < arr_size; i++) arr[i] = i;
			for (int i = 0; i < arr_size; i++) cout << arr[i] << " ";

			delete[] arr;
		}
		FN class_ex() {

			//�ı��� ȣ�� O
			{
				AAA a('a');
				AAA z;
			}
			cout << endl;

			//�ı��� ȣ�� X
			//�޸𸮰� �����ȴ�!!
			{
				AAA* ptr1 = new AAA;
				AAA* ptr2 = new AAA('c');
			}
		}
		FN malloc_new_difference() {
			// malloc()/free() -> function
			//malloc ���� ��ü �����ڰ� ȣ����� X
			{
				cout << "malloc()/free()" << endl;
				AAA* p1 = (AAA*)malloc(sizeof(AAA)); //������ ȣ�� X
				free(p1); //�ı��� ȣ�� X
			}
			cout << endl;

			// new/delete -> operator
			{
				cout << "new/delete" << endl;
				AAA* p1 = new AAA(); //������ ȣ��
				delete p1; //�ı��� ȣ��
				//�׸��� '������' -> ������ �����ε� ����!			
			}
			cout << endl;



		}
		FN new_del_oper_overload() /*�� ����*/  {
			int* p = new int;
			//int* p2 = new("stringinput", 2) int;

			new double[5];
			new string;

			new(p) int;

			delete p;
		}
		FN placement_new() /*�� ����*/ {

			{
				char reserve[1024] = "Hello!";
				cout << reserve << endl;

				//placement new
				//�޸𸮸� ���� �������� �� ũ�� �Ҵ��� �ʿ䰡 ���� �� ���
				klass* ptr = new (reserve) klass;

				cout << (void*)reserve << endl;
				cout << ptr << endl;

				cout << reserve << endl; //0x41424344(ABCD)�� overwrite��
				
			}
			cout << "\n\n\n";
			
			//�����ڸ� ������ ȣ���ϴ� ȿ���� ����
			klass k;
			//k.klass(); ������ ��ȣ�� �Ұ���
			k.~klass(); //�ı��ڴ� ������ ȣ�� ����
			k.~klass();
			k.~klass();
			k.~klass();

			new (&k) klass; //������ ȣ�� (���� �� ����)
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
			Point p3 = p1; //������ ȣ�� X (���� �����ڰ� ȣ���)
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

			shared_ptr<int> p3(new int, my_destructor_for_smart_ptr); //assign the �ı���
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

			//�ٸ� ��ü -> ���ü ����ȯ
			p1 = &i;
			cout << *p1 << endl;

			//���ü -> �ٸ� ��ü ����ȯ
			int* ptr = p1;
			cout << *ptr << endl;
		}
		FN type_casting_ex() {
			//c++������ ĳ������ ���� �����ڰ� 4���� �ִ�
			//����� ���� ���ø�ó�� �������� "������"�̴�
			//	static_cast<>		-> ������� ��ȯ�� �����
			//	reinterpret_cast<>	-> C ��Ÿ�� ĳ����(�޸� ���� ���ؼ�)
			//	const_cast<>		-> ��ü�� ����� ����
			//	dynamic_cast<>		-> �������� ����ȯ ������ �� ��� //RTTI���� �ٷ�

			double pi = 3.14;
			int* ptr = (int*)(&pi); //������ ������
			cout << *ptr << endl; //�۵��Ѵ�.

			//int* qtr = static_cast<int*>(&pi); ������ Ÿ�� ����

			//�޸� ���ؼ�(�޸� ������ �ʰ������� �ʴ´ٸ� ���α׷��� ����!)
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
			for (i = 0; i < 3; i++) cout << str[i] << " "; //a Z c ���
			cout << endl; 

			const int ARR[] = { 1, 2, 3 };
			const int* p_cARR = &ARR[1];
			int* p_ARR = const_cast<int*>(p_cARR);
			//ARR[1] = -200; unable; because const!
			*p_ARR = -200;
			for (i=0; i<3; i++) cout << ARR[i] << " "; //1 -200 3 ���
			cout << endl;

			const int INT = 1;
			const int* p_cINT = &INT;
			int* hack = const_cast<int*>(p_cINT);
			*hack = -100;
			cout << INT << endl; //1 ���
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

			//nullptr�� Ŭ��������! ������ ����ȯ ȣ���ϸ鼭 �ٸ����� ���� ��� Ŭ����
			nptr_ex_func(my_nullptr);
		}
		FN mem_ptr_ex() /*���� �˾ƺ����ҰŰ��ƿ�*/ {
			void (MyClass:: * mptr)(int);
			mptr = my_nullptr; // �̷��� ����Լ��� ����Ű�� �����ͷ� ��ȯ�� �����ϴ�.
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