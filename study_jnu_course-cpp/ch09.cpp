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
	};
	//operator+ �����ε� (���� ��ȯ��Ģ ���� �ʿ�)
	NumPair operator+(int i, NumPair& n) {
		return n + i;
	}


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
		}



	};
}

//main �Լ��� ���ӽ����̽��� ���θ� X
//�ü���� �׻� //::main()�� ȣ��
//int main() { CH09::RunExample::oper_overloading(); }