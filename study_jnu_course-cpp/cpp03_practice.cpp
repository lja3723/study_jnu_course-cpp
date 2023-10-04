#include <iostream>
using namespace std;
void cpp03_practice()
{
	//1. Box
	short length, width, height;
	cout << "길이 입력: "; cin >> length;
	cout << "너비 입력: "; cin >> width;
	cout << "높이 입력: "; cin >> height;
	cout << "상자의 부피: " << length * width * height << endl;

	//2. Timer
	int secs;
	cout << "초(시간)을 입력하세요: "; cin >> secs;
	int total_secs = secs;
	int days = secs / (60 * 60 * 24);
	secs %= (60 * 60 * 24);
	int hours = secs / (60 * 60);
	secs %= (60 * 60);
	int mins = secs / 60;
	secs %= 60;
	cout << total_secs << "초는 "
		<< days << "일 " << hours << "시간 "
		<< mins << "분 " << secs << "초입니다." << endl;

	//3. Digits
	short num;
	cout << "숫자를 입력하세요: "; cin >> num;
	cout << "천의 자리: " << num / 1000 << endl;
	num %= 1000;
	cout << "백의 자리: " << num / 100 << endl;
	num %= 100;
	cout << "십의 자리: " << num / 10 << endl;
	num %= 10;
	cout << "일의 자리: " << num << endl;
}