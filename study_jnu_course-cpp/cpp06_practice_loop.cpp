#include <iostream>
using namespace std;

//1. Order list
int cpp06_main1() {
	cout << "메뉴를 선택하세요(1~4 중 선택)" << endl;

	int input;
	int burger = 0, fries = 0, coke = 0;
	while (1) {
		cout << "1) 햄버거   2) 감자튀김   3) 콜라   4) 선택 종료\n";
		cin >> input;

		switch (input) {
		case 1: burger++; break;
		case 2: fries++; break;
		case 3: coke++; break;
		case 4: goto FINISH;
		default:
			cout << "잘못된 선택입니다." << endl;
			break;
		}
	}

FINISH:
	cout << "주문되었습니다." << endl;
	cout << "다음은 고객님의 주문입니다." << endl;

	if (burger) cout << "햄버거 * " << burger << endl;
	if (fries) cout << "감자튀김 * " << fries << endl;
	if (coke) cout << "콜라 * " << coke << endl;

	return 0;
}


//2. Times Table
int cpp06_main2() {
	for (int i = 1; i < 10; i++) {
		for (int j = 2; j < 10; j++)
			cout << j << "*" << i << "=" << (j * i) << "\t";
		cout << endl;
	}

	return 0;
}


//3. Lotto
#include <ctime>
int cpp06_main3() {
	srand(time(NULL));

	int n1, n2, n3, n4, n5, n6;
	n1 = n2 = n3 = n4 = n5 = n6 = 0;

	while (!n6) {
		int n = rand() % 45 + 1;
		if (!n1) n1 = n;
		if (n == n1) continue;
		if (!n2) n2 = n;
		if (n == n2) continue;
		if (!n3) n3 = n;
		if (n == n3) continue;
		if (!n4) n4 = n;
		if (n == n4) continue;
		if (!n5) n5 = n;	
		if (n == n5) continue;
		n6 = n;
	}

	cout << "로또 번호입니다." << endl;
	cout << n1 << endl << n2 << endl << n3 << endl;
	cout << n4 << endl << n5 << endl << n6 << endl;

	return 0;
}


//4. Diamond
int cpp06_main4() {
	int h;
	cout << "높이 입력: ";
	cin >> h;

	int k = (h + 1) / 2;
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k + i; j++) {
			cout << (j < k - i - 1 ? " " : "*");
		}
		cout << endl;
	}

	k = h / 2;
	int d1 = h % 2 ? 0 : -1;
	int d2 = h % 2 ? 1 : 0;
	for (int i = k; i > 0; i--) {
		for (int j = 0; j < k + i + d1; j++) {
			cout << (j < k - i + d2 ? " " : "*");
		}
		cout << endl;
	}

	return 0;
}


//5. Pascal's triangle (Bonus!)
int PascalTriangle(int i, int j) {
	static int v[100][100];

	if (i < j) return 0;
	if (j == 0 || i == j) return v[i][j] = 1;
	if (v[i][j] == 0)
		v[i][j] = PascalTriangle(i - 1, j - 1) + PascalTriangle(i - 1, j);

	return v[i][j];
}

int cpp06_main5() {
	
	int h;
	cout << "높이를 입력하세요: ";
	cin >> h;

	for (int i = 0; i < h; i++) {

		for (int j = 0; j < h - i - 1; j++)
			cout << "  ";

		for (int j = 0; j <= i; j++)
			cout << PascalTriangle(i, j) << "   ";
		cout << endl;
	}

	return 0;
}