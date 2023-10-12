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
	cout << "+ : ����" << endl;
	cout << "- : ����" << endl;
	cout << "* : ����" << endl;
	cout << "/ : ������" << endl;
	cout << "Q : ������" << endl;
	cout << "----------------" << endl;

	char op;
	cout << "�����ڸ� �Է��ϼ���: ";
	cin >> op;
	if (op == 'Q') {
		cout << "���α׷��� �����մϴ�." << endl;
		return -1;
	}
	if (op != '+' && op != '-' && op != '*' && op != '/') {
		cout << "���ǵ��� ���� �������Դϴ�." << endl;
		return -1;
	}

	double a, b;
	cout << "�� ���ڸ� �Է��ϼ���: ";
	cin >> a >> b;
	if (op == '/' && b == 0) {
		cout << "0���� ���� �� �����ϴ�." << endl;
		return -1;
	}

	cout << "��: " << adv_calc(a, b, op) << endl;
	return 0;
}


//2. Quadrant two
#include <cmath>
double dist_2d(int x1, int y1, int x2, int y2) {
	return pow(pow(x2 - x1, 2) + pow(y2 - y1, 2), 0.5);
}

int cpp08_main2() {
	int x1, x2, y1, y2;
	cout << "x1, y1 �Է�: ";
	cin >> x1 >> y1;
	cout << "x2, y2 �Է�: ";
	cin >> x2 >> y2;
	cout << "�Ÿ�: " << dist_2d(x1, y1, x2, y2) << endl;

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
	cout << "���ڸ� �Է��ϼ���: ";
	cin >> n;
	cout << "��ǥ�� �����ϸ� ������ �����ϴ�." << endl;
	print_digits_three(n);
	cout << endl;

	string n2;
	cout << "���ڸ� �Է��ϼ���(�ſ� �� ���ڵ� ����): ";
	cin >> n2;
	print_digits_three(n2);
	return 0;
}


//4. Tower of Hanoi
void hanoi_tower(int n, int start, int end, int via) {
	if (n == 0) return;

	hanoi_tower(n - 1, start, via, end);
	cout << n << "��° ��ũ�� " << start << "���� " << end << "(��)�� ";
	cout << via << "��(��) ���� �ű�ϴ�." << endl;
	hanoi_tower(n - 1, via, end, start);
}

int cpp08_main4() {
	int n;
	cout << "��ũ ���� �Է�: ";
	cin >> n;
	hanoi_tower(n, 1, 2, 3);

	return 0;
}