#include <iostream>
#include <climits>
using namespace std;
#define FN static void

#define DEF_PI 3.141592
#define ADD(X,Y) X = X+Y
#define SUB(X,Y) X = X-Y
#define PRT(X) cout << "Result: " << X << endl;
#define SQUARE(X) X * X
#define SQUARE_FIX(X) ((X) * (X))

class CH03 {
public:
	FN ch02() {
		int carrots;

		cin >> carrots;

		cout << "2�� �߰�";
		carrots = carrots + 2;

		cout << ": " << carrots;
	}
	FN initialize_variable() {
		//������ �ʱ�ȭ
		int a = 10; //copy initialization
		int b(20); //C++11, direct initialization
		int c{ 30 }; //c++11, uniform initialization
	}
	FN types_and_size() {
		cout << "�ڷ��� ũ��(����: byte)" << endl;
		cout << "bool: " << sizeof(bool) << endl;
		cout << "char: " << sizeof(char) << endl;
		cout << "wchar_t: " << sizeof(wchar_t) << endl;
		cout << "short: " << sizeof(short) << endl;
		cout << "int: " << sizeof(int) << endl;
		cout << "long: " << sizeof(long) << endl;
		cout << "long long: " << sizeof(long long) << endl;
		cout << "float: " << sizeof(float) << endl;
		cout << "double: " << sizeof(double) << endl;
		cout << "long double: " << sizeof(long double) << endl;

		cout << "\n�Ǵ� climits�� include�� ����� ����� �� �ִ�.\n";
		cout << "intsize: " << INT_MAX << endl;
		cout << "shortsize: " << SHRT_MAX << endl;
		cout << "longsize: " << LONG_MAX << endl;
		cout << "llsize: " << LLONG_MAX << endl;
	}
	FN overflow() {
		cout << "\n�����÷θ� �����غ���.\n";
		int intmax = 2147483647;
		cout << "1. intmax: " << intmax << endl;
		intmax = intmax + 1;
		cout << "2. intmax: " << intmax << endl;

		const double PI = 3.141592;
		cout << PI << endl;
	}
	FN realnum_precision() {
		float f = 0.0f;
		float g = 0.1f;
		for (int i = 0; i < 10; i++)
			f = f + g;

		//�Ҽ����� �����ϰ� ���� ���
		cout << fixed;
		cout.precision(9);

		//���� �߻�
		cout << f << endl;
		f = f - 1;
		cout << f << endl;
	}
	FN constant() {
		const double PI = 3.141592;
		cout << DEF_PI << endl;
		cout << PI << endl;
	}
	FN define_func() {
		int a = 10;
		int b = 5;

		ADD(a, b);
		PRT(a);
		SUB(a, b);
		PRT(a);

		//����!
		cout << SQUARE(5) << endl; // 5 * 5
		cout << SQUARE(2 + 3) << endl; // 2 + 3 * 2 + 3 = 11
		cout << SQUARE_FIX(2 + 3) << endl; // good
	}
	FN operators() {
		cout << 10 + 3 << endl;
		cout << 10 - 3 << endl;
		cout << 10 * 3 << endl;
		cout << 10 / 3 << endl;
		cout << (float)10 / 3 << endl; // ���� / ���� Ż�ǹ�
		cout << 10 % 3 << endl;
	}
};