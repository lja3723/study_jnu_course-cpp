#include <iostream>
using namespace std;

//1. Advanced calculator
int cpp05_main1() {
	cout << "----------------" << endl;
	cout << "+ : ����" << endl;
	cout << "- : ����" << endl;
	cout << "* : ����" << endl;
	cout << "/ : ������" << endl;
	cout << "Q : ������" << endl;
	cout << "----------------" << endl;

	char op;
	int n1, n2;
	cout << "�����ڸ� �Է��ϼ���: ";
	cin >> op;
	if (op != 'Q') {
		cout << "�� ���ڸ� �Է��ϼ���: ";
		cin >> n1 >> n2;
	}
	else {
		cout << "���α׷��� �����մϴ�." << endl;
		return 0;
	}

	cout << "��: ";
	switch (op) {
	case '+':
		cout << n1 + n2 << endl;
		return 0;
	case '-':
		cout << n1 - n2 << endl;
		return 0;
	case '*':
		cout << n1 * n2 << endl;
		return 0;
	case '/':
		if (n2 != 0)
			cout << double(n1) / n2 << endl;
		else
			cout << "0���� ���� �� �����ϴ�." << endl;
		return 0;
	default:
		cout << "���ǵ��� ���� �������Դϴ�." << endl;
	}
	return 0;
}


//2. Discount
int cpp05_main2() {
	int nums;
	cout << "������ ��ǰ ������ �Է��ϼ���: ";
	cin >> nums;

	if (nums < 10) {
		cout << "����: " << 1000 * nums << "��" << endl;
	}
	else {
		cout << "���� ����: " << 1000 * nums << "��" << endl;
		cout << "���ε� ����: " << 1000 * nums * 0.9 << "��" << endl;
	}
	return 0;
}


//3. Digits two
int cpp05_main3() {
	string words[10] = {
		"��", "�ϳ�", "��", "��", "��",
		"�ټ�", "����", "�ϰ�", "����", "��ȩ" };
	string units[4] = {
		"õ", "��", "��", "��" };

	int num;
	cout << "10000 �̸��� ���� �Է��ϼ���: ";
	cin >> num;
	for (int unit = 1000, i = 0; i < 4; unit /= 10, i++) {
		cout << units[i] << "���ڸ�: " << words[num / unit] << endl;
		num %= unit;
	}
	return 0;
}


//4. Minimum values
int cpp05_main4() {
	int n1, n2, n3;
	cout << "�� ���� �Է��ϼ���: ";
	cin >> n1 >> n2 >> n3;
	int min = n1 < n2 ? n1 : n2;
	min = min < n3 ? min : n3;
	cout << "���� ���� ��: " << min << endl;
	return 0;
}