#include <iostream>
using namespace std;

namespace CH09 {
#define FN static void

	namespace AAA {
		int i = 30;

		//���ӽ����̽��� ��÷
		namespace BBB {
			int i = 5050;
		}
	}

	int global = 20;
	int i = 20;

	//�ڵ��� ����� ��ü �и�
	//Header (Stack.h�� �ۼ�)
	class Stack_split {
	private:
		int stack[16];
		int sp;

	public:
		Stack_split();
		void push(int);
		int pop();
	};
	//Body (Stack.cpp�� �ۼ�)
	// :: -> scope operator (������ ������)
	Stack_split::Stack_split() : sp(0) {}
	void Stack_split::push(int value) {
		stack[sp++] = value;
	}
	int Stack_split::pop() {
		return stack[--sp];
	}

	class Stack_principle {
	public:
		int stack[16];
		int sp;

		Stack_principle() : sp(0) {}

		//��� ��������� ù �Ķ���ͷ� ClassName* const this�� ����
		//�����Ϸ��� �ڵ����� �߰�����

		//void push(Stack_principle* const this, int value) �� ����
		void push(int value) {
			//this->stack[(this->sp++)] = value; �� ����
			stack[sp++] = value; 
		}
		int pop() { return stack[--sp]; }
	};

	//Function Chaining example
	class Calc {
	private:
		int value;
	public:
		void setValue(int value) {
			this->value = value;
		}
		//�ڱ� �ڽ��� ������ �Ѱܼ� �ڱ������ �����ϰ� �Ѵ�
		Calc& Add(int v) {
			value += v;
			return *this;
		}
		//�����͸� ������ ���� ����
		//������ �Լ� ü�̴� �� -> �� �����ؾ� ��
		Calc* Sub(int v) {
			value -= v;
			return this;
		}
		Calc& Mul(int v) {
			value *= v;
			return *this;
		}
		void print() {
			cout << value << endl;
		}
	};

	class NumPair {
	private:
		int x, y;
	public:
		NumPair(int x, int y) : x(x), y(y) {}
		void print() {
			cout << "x: " << x << ", y: " << y << endl;
		}
		void add(int a, int b) {
			x += a; y += b;
		}
		int getX() { return x; }
		int getY() { return y; }
		NumPair operator+(NumPair& n) {
			return NumPair(x + n.getX(), y + n.getY());
		}
		NumPair operator+(int i) {
			return NumPair(x + i, y + i);
		}
		//����++ (Obj++) �� operator++(int)
		NumPair operator++(int) {
			NumPair ret = *this;
			x++, y++;
			return ret;
		}
		//++���� (++Obj) �� operator++()
		NumPair& operator++() {
			x++, y++;
			return *this;
		}
	};
	//operator+ �����ε� (���� ��ȯ��Ģ ���� �ʿ�)
	NumPair operator+(int i, NumPair& n) {
		return n + i;
	}

	//int�� ���� ���Ӱ� ������
	class INTEGER {
	private:
		int m_number;
	public:
		INTEGER(int n) : m_number(n) {}
		INTEGER() : INTEGER(0) {}
		friend ostream& operator<<(ostream& os, const INTEGER& n);
		//convention operator
		//INTEGER Ŭ���� ��ü�� int�� ��ȯ�Ѵ�
		operator int() { 
			return m_number;
		}
		INTEGER& operator=(int n) {
			m_number = n;
			return *this;
		}
		INTEGER operator+(const INTEGER& n) {
			return INTEGER(m_number + n.m_number);
		}
		INTEGER operator-(const INTEGER& n) {
			return INTEGER(m_number - n.m_number);
		}
		INTEGER operator*(const INTEGER& n) {
			return INTEGER(m_number * n.m_number);
		}
		INTEGER operator/(const INTEGER& n) {
			return INTEGER(m_number / n.m_number);
		}

		int* operator&() {
			return &m_number; //�� ���������ε� ��
		}
		//INTEGER& operator&() { return this; } //�����ε� �Ұ���..?
	};
	//INTEGER�� cout���� ���
	ostream& operator<<(ostream& os, const INTEGER& n) {
		os << n.m_number;
		return os;
	}

	//cout, endl�� ����
	//endl�� �Լ��� ������ ���̴� ����
	class myPrint {
	private:
		int len;

	public:
		void print(const char* str)	{ len += printf("%s ", str); }
		void print(int i)		{ len += printf("%d ", i); }
		void print(double d)	{ len += printf("%lf ", d); }

		myPrint& operator<<(const char* str) { print(str); return *this; }
		myPrint& operator<<(int i) { print(i); return *this; }
		myPrint& operator<<(double d) { print(d); return *this; }
		myPrint& operator<<(myPrint& (*f)(myPrint&)) {
			return f(*this);
		}
	};
	myPrint& endl(myPrint& p) { 
		printf("\n"); 
		return p;
	}
	myPrint p; // �̰��� �ٷ� cout

	class RunExample {
	public:
		FN intro() {
			Stack_split s;
			s.push(10);
			s.push(20);
			s.push(30);

			cout << s.pop() << endl;
			cout << s.pop() << endl;
			cout << s.pop() << endl;
		}
		FN using_global_var() {
			int CH09_global = -50;
			cout << CH09_global << endl; //�������� ����
			//cout << ::global << endl; //�������� ����

			//::global = 50;
			cout << CH09_global << endl;
			//cout << ::global << endl;
		}
		FN using_namespace() {
			int CH09_i = 10;
			cout << CH09_i << endl;
			//cout << ::i << endl; //��������(��� �̸����� namespace��)
			cout << AAA::i << endl; //���ӽ����̽� �� ����
			//cout << ::AAA::i << endl; //���� ����

			//���ӽ����̽��� ��ø
			cout << AAA::BBB::i << endl;
		}
		FN class_address() {
			Stack_principle s1, s2;
			s1.push(10);
			s2.push(20);
		}
		FN func_chaining_ex() {
			Calc c;
			c.setValue(10);
			c.Add(5).Sub(4)->Mul(3);
			c.print();

			Calc c2;
			c2.setValue(10);
			c2.Add(3).Sub(5)->Mul(4);
			c2.print();
		}
		FN oper_overloading() {
			NumPair num1(10, 20);
			NumPair num2(30, 40);

			//num1.add(30, 40); //������ �����ε� x
			cout << "num1 : "; num1.print();
			cout << "num2 : "; num2.print();

			num1 = num1 + num2;
			cout << "num1 : "; num1.print();
			cout << "num2 : "; num2.print();

			//num2 = num2 + -30; �Ʒ��� ����
			num2 = num2.operator+(-30);
			cout << "num2 : "; num2.print();
			//�Ʒ�ó�� ȣ���Ϸ��� �����Լ��� operator+ �����ε� �ʿ�
			num2 = 10 + num2; 
			cout << "num2 : "; num2.print();

			NumPair num3(5, 8);
			cout << "num3 : "; (num3++).print();
			cout << "num3 : "; (++num3).print();
		}
		FN myinteger_test() {
			INTEGER num;
			num = 10;
			cout << num << endl;

			int* pnum = &num; //���������� ���� �� ��

			cout << *pnum << endl;
		}
		FN printf_test() {
			int ret;
			ret = printf("ABCDEF"); //����� ���� ���� ������
			printf(" %d \n", ret);
		}
		FN myprint_test() {
			myPrint p;
			char str[15] = "world!";
			
			p.print("hello");
			p.print(str);
			p.print(-123);
			p.print(-3.14);
			cout << std::endl;
			//cout << endl; �̷��� �ϸ� std::endl�� CH09::endl�� �ߺ��Ǽ� �̻��Ѱ� ��µ�

			p << "hello" << str << 1234 << 3.14 << endl << 5678;
		}


	};
}

//main �Լ��� ���ӽ����̽��� ���θ� X
//�ü���� �׻� //::main()�� ȣ��
int main() { CH09::RunExample::myprint_test(); }