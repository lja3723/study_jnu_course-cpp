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
		int* p;
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
	
	struct address {
		string city;
		string country;
		int zipcode;
	};
	struct member {
		int id;
		string name;
		address addr;
	};
	struct myarray {
		int x[7];
	};
	static void avoid_array_decay_test(myarray arr) {
		cout << sizeof(arr) << endl;
		cout << sizeof(arr.x) << endl;
		cout << sizeof(arr.x[0]) << endl;
		cout << sizeof(arr.x) / sizeof(*arr.x) << endl;
		cout << endl;

		cout << arr.x[0] << "\n" << arr.x[1] << endl;
		cout << arr.x[2] << "\n" << arr.x[3] << endl;
	}

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
	FN struct_combined_size() {
		member m;
		m.id = 1234;
		m.name = "LJA";
		m.addr.city = "GY";
		m.addr.country = "S.Korea";
		m.addr.zipcode = 61186;
		cout << sizeof(string) << endl;

		cout << "sizeof(address): " << sizeof(address) << endl;
		cout << "sizeof(member): " << sizeof(member) << endl;
	}
	FN avoid_array_decay() {
		myarray brr = { {10, 20, 30, 40 } };
		avoid_array_decay_test(brr);
	}
	FN midterm_test() {
		unsigned int i;
		while (i++);
		cout << i << endl;
	}


};