#include <iostream>
using namespace std;
void cpp03_practice()
{
	//1. Box
	short length, width, height;
	cout << "���� �Է�: "; cin >> length;
	cout << "�ʺ� �Է�: "; cin >> width;
	cout << "���� �Է�: "; cin >> height;
	cout << "������ ����: " << length * width * height << endl;

	//2. Timer
	int secs;
	cout << "��(�ð�)�� �Է��ϼ���: "; cin >> secs;
	int total_secs = secs;
	int days = secs / (60 * 60 * 24);
	secs %= (60 * 60 * 24);
	int hours = secs / (60 * 60);
	secs %= (60 * 60);
	int mins = secs / 60;
	secs %= 60;
	cout << total_secs << "�ʴ� "
		<< days << "�� " << hours << "�ð� "
		<< mins << "�� " << secs << "���Դϴ�." << endl;

	//3. Digits
	short num;
	cout << "���ڸ� �Է��ϼ���: "; cin >> num;
	cout << "õ�� �ڸ�: " << num / 1000 << endl;
	num %= 1000;
	cout << "���� �ڸ�: " << num / 100 << endl;
	num %= 100;
	cout << "���� �ڸ�: " << num / 10 << endl;
	num %= 10;
	cout << "���� �ڸ�: " << num << endl;
}