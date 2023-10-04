#include <iostream>
#include <ctime>
using namespace std;
#define FN static void

//템플릿 메타 프로그래밍
template <int N>
struct CH05_FACT {
	enum { value = N * CH05_FACT<N - 1>::value };
};

template<>
struct CH05_FACT<0> {
	enum { value = 1 };
};


class CH05 {
public:
	static string save(string in) {
		static string a;
		return in != "" ? a = in : a;
	}
	static void func2(int i, int j) {
		int k;
	}
	static void func1() {
		int x = 0, y = 0, z;
		func2(x, y);
	}
	static int add(int a, int b) {
		return a + b;
	}
	static double add(double a, double b) {
		return a + b;
	}
	static int add(int a, int b, int c) {
		return a + b + c;
	}
	static double add(double a, double b, double c) {
		return a + b + c;
	}
	template <typename T>
	static T tadd(T a, T b) {
		return a + b;
	}


	FN scope_test() {
		int x = 10;

		{
			int x = 1;
			x++;
			cout << x << endl;
		}

		cout << x << endl;
	}
	FN static_test() {
		save("값을 저장하고 싶어용");
		cout << save("") << endl;
	}
	FN func_stack() {
		int a, b, c;
		func1();
	}
	FN static_test_2() {
		for (int i = 0; i < 5; i++) {
			static int temp;
			cout << temp++ << endl;
		}
		//cout << "out: " << temp << endl;
	}
	FN func_overloading() {
		cout << add(10, 20) << endl;
		cout << add(10.3, 20.4) << endl;
		cout << add(10, 20, 30) << endl;
		cout << add(10.3, 20.4, 15.8) << endl;
	}
	FN template_meta() {
		int a = CH05_FACT<20>::value;
		cout << a;
	}
};