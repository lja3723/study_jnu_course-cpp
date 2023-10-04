#include <iostream>
using namespace std;

//1. Caesar cipher
int cpp04_main_1() {
	string str;
	int key;
	cout << "8�ڸ��� �빮�� ���ڿ� �Է�: ";
	cin >> str;
	cout << "10 �̸��� ���� �Է�: ";
	cin >> key;

	cout << "Cipher ���ڿ�: ";
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
	cout << "8�ڸ��� �빮�� ���ڿ� �Է�: ";
	cin >> str;
	cout << "10 �̸��� ���� �Է�: ";
	cin >> key;
	key = -key;

	cout << "�ص� ���ڿ�: ";
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
	cout << "x �Է�: ";
	cin >> x;
	cout << "y �Է�: ";
	cin >> y;

	cout << "�ش� ��ǥ�� ";
	int ret = (x > 0 && y > 0) ? 1 :
		(x < 0 && y > 0) ? 2 :
		(x < 0 && y < 0) ? 3 :
		(x > 0 && y < 0) ? 4 : 0;
		
	ret ?
		cout << "��" << ret << "�и鿡 ��ġ�� �ֽ��ϴ�." << endl :
		cout << "��� ��и鿡�� ��ġ���� �ʽ��ϴ�." << endl;

	return 0;
}

//4. Population
int cpp04_main_4() {
	long long pw, pk;

	cout << fixed;
	cout.precision(5);

	cout << "���� �α� �� �Է�: ";
	cin >> pw;
	cout << "���ѹα� �α� �� �Է�: ";
	cin >> pk;
	cout << "���ѹα� �α� ���� ���� �α� ���� ";
	cout << (double)pk / pw * 100 << "%�Դϴ�." << endl;
	return 0;
}

//5. Your height
int cpp04_main_5() {
	int height;
	cout << "������ Ű(cm)�� �Է��ϼ���: ";
	cin >> height;
	double inch = height / 2.54;
	int feet = inch / 12;
	while (inch >= 12)
		inch -= 12;

	cout << height << "cm�� ";
	cout << feet << "��Ʈ " << inch << "��ġ�Դϴ�." << endl;

	return 0;
}