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


	//TODO: ���� ���� ���� ã��
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

		Point(const Point& p) : x(p.x), y(p.y), name(p.name) /*���� ����!!*/ {
			this->name = new char[strlen(p.name)];
			strcpy(this->name, p.name);	//���� ����!
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
			int* p = new int(5);
			int* p2 = new(sizeof(2), "new new new", 2) int;

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




	};


}

//int main() { CH10::RunExample::explicit_implicit_call(); }