#include <iostream>
using namespace std;

namespace CH09 {
#define FN static void

	namespace AAA {
		int i = 30;

		//네임스페이스의 중첨
		namespace BBB {
			int i = 5050;
		}
	}

	int global = 20;
	int i = 20;

	//코드의 헤더와 몸체 분리
	//Header (Stack.h에 작성)
	class Stack_split {
	private:
		int stack[16];
		int sp;

	public:
		Stack_split();
		void push(int);
		int pop();
	};
	//Body (Stack.cpp에 작성)
	// :: -> scope operator (스코프 연산자)
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

		//모든 멤버변수는 첫 파라미터로 ClassName* const this를 가짐
		//컴파일러가 자동으로 추가해줌

		//void push(Stack_principle* const this, int value) 와 동일
		void push(int value) {
			//this->stack[(this->sp++)] = value; 와 동일
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
		//자기 자신의 참조를 넘겨서 자기수정이 가능하게 한다
		Calc& Add(int v) {
			value += v;
			return *this;
		}
		//포인터를 리턴할 수도 있음
		//하지만 함수 체이닝 시 -> 로 접근해야 됨
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
	//operator+ 오버로딩 (덧셈 교환법칙 위해 필요)
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
			cout << CH09_global << endl; //지역변수 참조
			//cout << ::global << endl; //전역변수 참조

			//::global = 50;
			cout << CH09_global << endl;
			//cout << ::global << endl;
		}
		FN using_namespace() {
			int CH09_i = 10;
			cout << CH09_i << endl;
			//cout << ::i << endl; //전역변수(사실 이름없는 namespace다)
			cout << AAA::i << endl; //네임스페이스 안 변수
			//cout << ::AAA::i << endl; //위와 동일

			//네임스페이스의 중첩
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

			//num1.add(30, 40); //연산자 오버로딩 x
			cout << "num1 : "; num1.print();
			cout << "num2 : "; num2.print();

			num1 = num1 + num2;
			cout << "num1 : "; num1.print();
			cout << "num2 : "; num2.print();

			//num2 = num2 + -30; 아래와 동일
			num2 = num2.operator+(-30);
			cout << "num2 : "; num2.print();
			//아래처럼 호출하려면 전역함수로 operator+ 오버로딩 필요
			num2 = 10 + num2; 
			cout << "num2 : "; num2.print();
		}



	};
}

//main 함수는 네임스페이스로 감싸면 X
//운영체제는 항상 //::main()을 호출
//int main() { CH09::RunExample::oper_overloading(); }