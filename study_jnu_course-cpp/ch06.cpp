#include <iostream>
using namespace std;
#define FN static void


class CH06 {
	struct position_1 {
		int x;
		int y;
	};
	struct position_2 {
		double x;
		double y;
	};
	struct the_size {
		short s;

		long long l;
		int i;
	};
	struct mystruct {
		short x;
		short y;
	};
	union one4all
	{
		int in;
		int lo;
		double dou;
	};

public:
	FN array_intro() {
		char str[] = "Hello!";
		int stack[16];

		cout << sizeof(str) << endl << sizeof(stack) << endl;
	}
	FN init_array() {
		//모던 C++ 문법
		//int arr6[1000] = { 0, [980] = 30 }; (VS에선 안되는듯)
	}
	FN array_2d() {
		int num[3][3] = {
			{1, 2, 3},
			{4, 5, 6},
			{7, 8, 9}
		};

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				cout << num[i][j] << " ";
		cout << endl;

		//1차원 배열로 접근하기
		for (int i = 0; i < 9; i++)
			cout << (((int*)num)[i]) << " ";
		cout << endl;
		for (int i = 0; i < 9; i++)
			cout << num[0][i] << " ";
		cout << endl;
	}
	FN kk() {
		int pass[8];
		int input[8];
	}
	FN struct_sizeof() {
		cout << sizeof(position_1) << endl;
		cout << sizeof(position_2) << endl;
		cout << sizeof(the_size) << endl;
	}
	FN struct_padding_test() {
		mystruct a;
		a.x = 0xAABB;
		a.y = 0xCCDD;

		cout << hex;
		cout << a.x << endl;
		cout << a.y << endl;

		cout << *((int *)(&a)) << endl;
	}
	FN union_ex() {
		one4all uni;
		uni.in = 10;
		uni.lo = 20;
		uni.dou = 30;
		cout << uni.in << endl;
		cout << uni.lo << endl;
		cout << uni.dou << endl;
	}



};

//int main() { CH06::union_ex(); }