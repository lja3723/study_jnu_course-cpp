#include <iostream>
using namespace std;

//1. Caesar cipher
int cpp04_main_1() {
	string str;
	int key;
	cout << "8자리의 대문자 문자열 입력: ";
	cin >> str;
	cout << "10 미만의 정수 입력: ";
	cin >> key;

	cout << "Cipher 문자열: ";
	cout << (char)((str[0] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[1] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[2] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[3] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[4] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[5] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[6] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[7] - 'A' + key + 52) % 26 + 'A') << endl;

	return 0;
}

//2. Decryption
int cpp04_main_2() {
	string str;
	int key;
	cout << "8자리의 대문자 문자열 입력: ";
	cin >> str;
	cout << "10 미만의 정수 입력: ";
	cin >> key;
	key = -key;

	cout << "해독 문자열: ";
	cout << (char)((str[0] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[1] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[2] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[3] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[4] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[5] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[6] - 'A' + key + 52) % 26 + 'A');
	cout << (char)((str[7] - 'A' + key + 52) % 26 + 'A') << endl;

	return 0;
}

//3. Quadrant
int cpp04_main_3() {
	int x, y;
	cout << "x 입력: ";
	cin >> x;
	cout << "y 입력: ";
	cin >> y;

	cout << "해당 좌표는 ";
	int ret = (x > 0 && y > 0) ? 1 :
		(x < 0 && y > 0) ? 2 :
		(x < 0 && y < 0) ? 3 :
		(x > 0 && y < 0) ? 4 : 0;
		
	ret ?
		cout << "제" << ret << "분면에 위치해 있습니다." << endl :
		cout << "어느 사분면에도 위치하지 않습니다." << endl;

	return 0;
}

//4. Population
int cpp04_main_4() {
	long long pw, pk;

	cout << fixed;
	cout.precision(5);

	cout << "세계 인구 수 입력: ";
	cin >> pw;
	cout << "대한민국 인구 수 입력: ";
	cin >> pk;
	cout << "대한민국 인구 수는 세계 인구 수의 ";
	cout << (double)pk / pw * 100 << "%입니다." << endl;
	return 0;
}

//5. Your height
int cpp04_main_5() {
	int height;
	cout << "본인의 키(cm)를 입력하세요: ";
	cin >> height;
	double inch = height / 2.54;
	int feet = inch / 12;
	while (inch >= 12)
		inch -= 12;

	cout << height << "cm는 ";
	cout << feet << "피트 " << inch << "인치입니다." << endl;

	return 0;
}