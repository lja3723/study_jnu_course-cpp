#include <iostream>
using namespace std;
#define FN static void


class CH05 {
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



};

int main() { CH05::array_2d(); }