#include <iostream>
using namespace std;


//1. More advanced calculator
template <typename T>
T adv_calc(T n1, T n2, char op) {
	switch (op) {
	case '+':
		return n1 + n2;
	case '-':
		return n1 - n2;
	case '*':
		return n1 * n2;
	case '/':
		return n1 / n2;
	}
	return 0;
}

int cpp08_main1() {
	cout << "----------------" << endl;
	cout << "+ : 덧셈" << endl;
	cout << "- : 뺄셈" << endl;
	cout << "* : 곱셈" << endl;
	cout << "/ : 나눗셈" << endl;
	cout << "Q : 나가기" << endl;
	cout << "----------------" << endl;

	char op;
	cout << "연산자를 입력하세요: ";
	cin >> op;
	if (op == 'Q') {
		cout << "프로그램을 종료합니다." << endl;
		return -1;
	}
	if (op != '+' && op != '-' && op != '*' && op != '/') {
		cout << "정의되지 않은 연산자입니다." << endl;
		return -1;
	}

	double a, b;
	cout << "두 숫자를 입력하세요: ";
	cin >> a >> b;
	if (op == '/' && b == 0) {
		cout << "0으로 나눌 수 없습니다." << endl;
		return -1;
	}

	cout << "답: " << adv_calc(a, b, op) << endl;
	return 0;
}


//2. Quadrant two
#include <cmath>
double dist_2d(int x1, int y1, int x2, int y2) {
	return pow(pow(x2 - x1, 2) + pow(y2 - y1, 2), 0.5);
}

int cpp08_main2() {
	int x1, x2, y1, y2;
	cout << "x1, y1 입력: ";
	cin >> x1 >> y1;
	cout << "x2, y2 입력: ";
	cin >> x2 >> y2;
	cout << "거리: " << dist_2d(x1, y1, x2, y2) << endl;

	return 0;
}


//3. Digits three
#include <iomanip>
void print_digits_three(long long n) {
	if (n < 1000)
		cout << n;
	else {
		print_digits_three(n / 1000);
		cout << "," << setw(3) << setfill('0') << (n % 1000);
	}
}
void print_digits_three(string n) {
	if (n.length() < 4) {
		cout << n;
	}
	else {
		print_digits_three(n.substr(0, n.length() - 3));
		cout << "," << n.substr(n.length() - 3, 3);
	}
}

int cpp08_main3() {
	long long n;
	cout << "숫자를 입력하세요: ";
	cin >> n;
	cout << "쉼표로 구분하면 다음과 같습니다." << endl;
	print_digits_three(n);
	cout << endl;

	string n2;
	cout << "숫자를 입력하세요(매우 긴 숫자도 가능): ";
	cin >> n2;
	print_digits_three(n2);
	return 0;
}


//4. Tower of Hanoi
void hanoi_tower(int n, int start, int end, int via) {
	if (n == 0) return;

	hanoi_tower(n - 1, start, via, end);
	cout << n << "번째 디스크를 " << start << "에서 " << end << "(으)로 ";
	cout << via << "을(를) 거쳐 옮깁니다." << endl;
	hanoi_tower(n - 1, via, end, start);
}

int cpp08_main4() {
	int n;
	cout << "디스크 개수 입력: ";
	cin >> n;
	hanoi_tower(n, 1, 2, 3);

	return 0;
}