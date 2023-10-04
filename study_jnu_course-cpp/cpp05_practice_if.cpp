#include <iostream>
using namespace std;

//1. Advanced calculator
int cpp05_main1() {
	cout << "----------------" << endl;
	cout << "+ : 덧셈" << endl;
	cout << "- : 뺄셈" << endl;
	cout << "* : 곱셈" << endl;
	cout << "/ : 나눗셈" << endl;
	cout << "Q : 나가기" << endl;
	cout << "----------------" << endl;

	char op;
	int n1, n2;
	cout << "연산자를 입력하세요: ";
	cin >> op;
	if (op != 'Q') {
		cout << "두 숫자를 입력하세요: ";
		cin >> n1 >> n2;
	}
	else {
		cout << "프로그램을 종료합니다." << endl;
		return 0;
	}

	cout << "답: ";
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
			cout << "0으로 나눌 수 없습니다." << endl;
		return 0;
	default:
		cout << "정의되지 않은 연산자입니다." << endl;
	}
	return 0;
}


//2. Discount
int cpp05_main2() {
	int nums;
	cout << "구매할 물품 개수를 입력하세요: ";
	cin >> nums;

	if (nums < 10) {
		cout << "가격: " << 1000 * nums << "원" << endl;
	}
	else {
		cout << "원래 가격: " << 1000 * nums << "원" << endl;
		cout << "할인된 가격: " << 1000 * nums * 0.9 << "원" << endl;
	}
	return 0;
}


//3. Digits two
int cpp05_main3() {
	string words[10] = {
		"공", "하나", "둘", "셋", "넷",
		"다섯", "여섯", "일곱", "여덟", "아홉" };
	string units[4] = {
		"천", "백", "십", "일" };

	int num;
	cout << "10000 미만의 수를 입력하세요: ";
	cin >> num;
	for (int unit = 1000, i = 0; i < 4; unit /= 10, i++) {
		cout << units[i] << "의자리: " << words[num / unit] << endl;
		num %= unit;
	}
	return 0;
}


//4. Minimum values
int cpp05_main4() {
	int n1, n2, n3;
	cout << "세 수를 입력하세요: ";
	cin >> n1 >> n2 >> n3;
	int min = n1 < n2 ? n1 : n2;
	min = min < n3 ? min : n3;
	cout << "가장 작은 수: " << min << endl;
	return 0;
}