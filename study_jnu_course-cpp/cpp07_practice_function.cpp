#include <iostream>
using namespace std;


//1. Lotto game!
void cpp07_get_input(int _out_input[]) {
	//사용자 입력 받기
	int freq[46] = { 0, };
	cout << "로또 번호를 추측해 보세요:" << endl;
	for (int i = 0; i < 6; i++) {
		int n; cin >> n;
		if (!(1 <= n && n <= 45)) {
			cout << "1~45 범위어야 합니다. 다시 입력하세요: ";
			i--;
		}
		else if (freq[n]) {
			cout << "번호가 중복되었습니다. 다시 입력하세요: ";
			i--;
		}
		else {
			freq[n]++;
			_out_input[i] = n;
		}
	}
	cout << "입력한 번호입니다: ";
	for (int i = 0; i < 6; i++)
		cout << _out_input[i] << " ";
	cout << endl;
}

void cpp07_generate_nums(int _out_nums[]) {
	//로또 번호 생성
	srand(100);
	cout << "로또 번호입니다: ";
	int freq[46] = {0, };
	for (int i = 0; i < 6; i++) {
		int n = rand() % 45 + 1;
		if (freq[n])
			i--;
		else {
			_out_nums[i] = n;
			cout << _out_nums[i] << " ";
		}
	}
	cout << endl;
}

void cpp07_calc_rank(int _in_input[], int _in_nums[]) {
	int matches = 0;
	//일치 개수 계산
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 6; j++)
			if (_in_input[i] == _in_nums[j]) matches++;

	//랭크 계산
	int ranks[7] = { 0, 0, 0, 5, 4, 3, 1 };
	cout << "숫자 " << matches << "개가 일치합니다";
	if (!ranks[matches])
		cout << ". " << "꽝입니다... 다음에 다시 도전해주세요." << endl;
	else
		cout << "! " << ranks[matches] << "등입니다!" << endl;
}

int cpp07_main1() {
	int input[6], nums[6];
	cpp07_get_input(input);
	cpp07_generate_nums(nums);
	cpp07_calc_rank(input, nums);
	return 0;
}


//2. Prime number
#include <cmath>
bool cpp07_check_prime_number(int n) {
	if (n < 2) return false;
	if (n == 2) return true;
	if (n % 2 == 0) return false;

	int check_range = sqrt(n);
	for (int k = 3; k <= check_range; k += 2) {
		if (n % k == 0)
			return false;
	}
	return true;
}

int cpp07_main2() {
	while (true) {
		cout << "숫자를 입력하세요: ";
		int input;
		cin >> input;
		if (input == -1)
			break;

		cout << input << "은(는) 소수";
		if (cpp07_check_prime_number(input))
			cout << "입니다.";
		else
			cout << "가 아닙니다.";
		cout << endl << endl;
	}

	cout << "프로그램을 종료합니다." << endl;
	return 0;
}


//3. ipower()
long long cpp07_ipower(int n, int k) {
	if (k < 0) return 0;
	if (k == 0) return 1;
	if (k == 1) return n;
	int half = cpp07_ipower(n, k / 2);
	return half * half * (k % 2 ? n : 1);
}

int cpp07_main3() {
	cout << "자연수를 입력하세요: ";
	int input; cin >> input;
	for (int i = 0; i <= 10; i++)
		cout << input << "^" << i << " = " << cpp07_ipower(input, i) << endl;
	return 0;
}


//4. Probability
void cpp07_graph(int len, int dot) {
	for (int i = 0; i < len; i++)
		cout << (i < dot ? "@" : "*");
	cout << endl;
}

int cpp07_main4() {
	cout << "주사위를 굴릴 횟수를 입력하세요: ";
	int rolls; cin >> rolls;
	int freq[6] = { 0, };
	srand(100);
	for (int i = 0; i < rolls; i++)
		freq[rand() % 6]++;

	//표시되는 문자 개수를 제한한다.
	int show_chars = 64;
	for (int i = 0; i < 6; i++) {
		cout << (i + 1) << ":";
		cpp07_graph(show_chars, freq[i] * show_chars / rolls);
	}

	return 0;
}