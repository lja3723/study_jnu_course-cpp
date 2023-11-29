#include <iostream>
#include <list>
#include <vector>
using namespace std;

namespace CH12 {
#define FN static void
	class Animal {
		string howl;
	public:
		Animal() { howl = ""; }
		void cry() { cout << howl << endl; };
		void eat() { cout << "Feed feed!" << endl; }
		void run() { cout << "Run Run!" << endl; }

		Animal(const string& sound) : howl(sound) {}
	};

	class Cat : public Animal {
	public:
		Cat(const string& sound) : Animal(sound) {}
		void nyang() { cout << "Cat cat cat" << endl; }
	};

	class Cow : public Animal {
		int val;
	public:
		Cow(const string& sound) : val(5), Animal(sound) {}
		void Umme() { cout << "Cow Cow Cow" << endl; }
	};

	class Dog : public Animal {
		Dog() : Animal() {}
	public:
		Dog(const string& sound) : Animal(sound) { cout << "Dog" << endl; }
		Dog(int age) : Dog("") { cout << "Dog aged " << age << endl; }
		void bark() { cout << "Mung!" << endl; }
	};

	class MyDog : public Dog {
	public:
		//MyDog() {} compiler error
		MyDog(const string& s) : Dog(s) {}
		MyDog(int age) : Dog(age) {}
		void bark() { cout << "Wal!" << endl; }
		void bark(bool feed) { cout << "Krre re ek mmd kkaksdf" << endl; }
	};

	class ctrlId {
	private:
		int i;
	protected:
		int j;
	public:
		int k;

	public:
		ctrlId() {}
		ctrlId(int a, int b, int c) : i(a), j(b), k(c) {};

		void print() {
			cout << i << endl;
			cout << j << endl;
			cout << k << endl;
		}
	};

	class ctrlIdTest : private ctrlId {
	public:
		ctrlIdTest(int a, int b, int c) : ctrlId(a, b, c) {}

		void qrint() {
			//cout << i << endl;
			cout << j << endl;
			cout << k << endl;
		}
	};

	class ctrlIdTest2 : public ctrlIdTest {
	public:

		ctrlIdTest2(int a, int b, int c) : ctrlIdTest(a, b, c) {}

		void rrint() {
			//cout << i << endl;
			//cout << j << endl;
			//cout << k << endl;
		}
	};

	template<class T, class U = list<T>> class stack {
	private:
		U st;
	public:
		void push(const T& a)	{ st.push_back(a); }
		void pop()				{ st.pop_back(); }
		T&	 top()				{ return st.back(); }
	};

	
	class RunExample {
	public:
		FN func() {
			Dog d("mung mung");
			Cat c("nyang nyang");
			Cow w("Ummmmmmmmmmmmmmeeeeeeeeeeeeee");
			d.cry();
			c.cry();
			w.cry();


		}
		FN func2() {
			Dog dog("");
			MyDog myDog("");
			MyDog mDog(5);

			dog.bark();
			myDog.bark();
			myDog.bark(true);


		}
		FN ctrlIdTest() {
			ctrlIdTest2 t(1, 2, 3);
			//cout << t.i << endl;
			//cout << t.j << endl;
			//cout << t.k << endl;

			t.qrint();


		}
		FN has_a_test() {
			stack<int, vector<int>> s;
			stack<int, list<int>> ss;
			//stack<int> sss;

			s.push(10);
			s.push(20);
			s.push(30);
			
			cout << s.top() << endl; s.pop();
			cout << s.top() << endl; s.pop();
			cout << s.top() << endl; s.pop();
		}
	};
}

namespace CH12_2 {
#define FN static void

	class Animal {
	public:
		virtual/*���� ���ε��� �����ϰ� �ϴ� Ű����*/
			void Cry() /*= 0*/ {}; //���� �����Լ� /*{ cout << "uh...?" << endl; }*/
		virtual void Eat() /*= 0*/ {};
		virtual void Sleep() /*= 0*/ {};

		//��� �θ��� �Ҹ��ڴ� �����̾�� �ȴ�~!
		virtual ~Animal() { cout << "Animal destructor" << endl; }
	};

	class Dog : public Animal {
	public:
		int dog_color; //���� ����!!
		void bark() { cout << "Wal! Wal!!" << endl; }

		//���� ���� �Լ��� �������̵� �ݵ�� �ʿ�
		void Cry() override { cout << "Mung mung" << endl; }
		void Eat() override { cout << "bone bone" << endl; };
		void Sleep() override { cout << "Zzz.. Zzz..." << endl; };

		virtual ~Dog() { cout << "Dog destructor" << endl; }
	};

	class Cat : public Animal {
	public:
		void Cry() override { cout << "Nyang~" << endl; }
		void Eat() override { cout << "fish fish" << endl; };
		void Sleep() override { cout << "Zzz.. Zzz..." << endl; };
	};

	class Cow : public Animal {
	public:
		void Cry() override { cout << "Ummmme~~~" << endl; }
		void Eat() override { cout << "grass grass" << endl; };
		void Sleep() override { cout << "Zzz.. Zzz..." << endl; };
	};

	void foo(Animal* p) {
		p->Cry();
	}

	void FrameworkMain(Animal* a) {
		a->Cry();
		a->Eat();
		a->Sleep();
	}



	class PPP {
		void* vptr; //8 (virtual �߰��� ������)

		int a;
		int b;

	public:
		//����Ʈ �Ķ���ʹ� virtual�� ������ �ȵ�
		virtual void func(int a = 10) {
			cout << "ppp: " << a << endl;
		}
	};

	class CCC : public PPP {
		int x;
	public:
		virtual void func(int a = 20) {
			cout << "ccc: " << a << endl;
		}
	};



	void fooo(Animal* p) {
		const type_info& t1 = typeid(*p);
		const type_info& t2 = typeid(Dog);

		if (t1 == t2) {
			cout << "p is Dog!" << endl;
			Dog* pDog = static_cast<Dog*>(p);

			pDog->dog_color = 1;
			pDog->bark();
			cout << "My color has been changed!" << endl;
		}
	}

	void fooo_dynamic_cast(Animal* p) {
		//static_cast �� �ٿ�ĳ���� üũ �Ұ���
		//Dog* pDog = static_cast<Dog*>(p);


		Dog* pDog = dynamic_cast<Dog*>(p);
		//�ٿ�ĳ������ ��� 0 ����
		//��ĳ������ ���( typeid(*p) == typeid(Dog) ) ����ȯ �Ϸ�

		cout << pDog << endl;

		if (pDog != nullptr) {
			cout << "p is Dog!" << endl;
			pDog->dog_color = 10;
			pDog->bark();
			cout << "My color has been changed!" << endl;
		}
	}




	class RunExample {
	public:
		FN virtual_ex() {
			list<Dog*> s1; //store Dog class only
			list<Animal*> s2; //stor all of animals

			s2.push_back(new Dog());
			s2.push_back(new Dog());
			s2.push_back(new Cow());
			s2.push_back(new Cat());
			s2.push_back(new Cat());
			s2.push_back(new Cow());
			s2.push_back(new Dog());
			s2.push_back(new Cat());
			s2.push_back(new Cow());
			s2.push_back(new Cow());
			s2.push_back(new Dog());
			s2.push_back(new Cat());

			for (auto i : s2) {
				i->Cry();
			}


			
		}
		FN virtual_ex_2() {
			PPP* p = new CCC;
			PPP* p1 = new PPP;

			p1->func();
			p->func(); //���� �����?

			//ccc: 10�� ��
			//����
			//vptr�� ����ִ� �Լ��� �����ϴµ�,
			//���� �����ʹ� PPP�̴ϱ� �Ķ���ʹ� PPP�� �⺻ �Ķ���͸� �������� ��


			cout << sizeof(PPP) << endl; 
				//8(no virtual)
				//16(virtual) ->
			cout << sizeof(CCC) << endl; 
				//12(no virtual)
				//24(virtual) = 8 + 4(padding)


		}
		FN pure_virtual_class_ex() {
			//Animal aa; //�Ұ���
			Dog dog; //����� ����
		}
		FN RTTI_ex() {
			//Java, C#���� reflection�̶� �Ҹ��� ��
			Animal a;
			Dog d;

			cout << typeid(Animal).name() << endl;
			cout << typeid(a).name() << endl;
			cout << typeid(Dog).name() << endl;
			cout << typeid(d).name() << endl;

			Animal* pa = &a;
			const type_info & t1 = typeid(*pa);
			cout << t1.name() << endl;

			pa = &d;
			const type_info& t2 = typeid(*pa);
			cout << t2.name() << endl;

		}
		FN dynamic_cast_ex() {
			Animal a;
			Dog d;
			void (*func)(Animal*) = fooo_dynamic_cast;

			func(&a);
			func(&d);


		}
		FN virtual_destructor_ex() {
			Animal* a = new Dog;

			a->Cry();

			delete a;
			//�Ǵ� �ڽ����� ����ȯ �����ؼ� ���� ����
			//delete static_cast<Dog*>(a);
		}
		FN virtual_destructor_ex2() {
			//������ ���� ��ü �ʱ�ȭ�ϸ�?
			//Dog* p = new Dog;
			//vptr�� �ʱ�ȭ �ȵ� -> ���̳��� �ε� �Ұ���
			Dog* p = static_cast<Dog*>(malloc(sizeof(Dog)));
			p->Cry();
		}



	};
}

int main() { CH12_2::RunExample::virtual_destructor_ex2(); }