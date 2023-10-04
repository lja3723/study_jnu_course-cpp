#include <iostream>
using namespace std;


//1. Lotto game!
void cpp07_get_input(int _out_input[]) {
	//����� �Է� �ޱ�
	int freq[46] = { 0, };
	cout << "�ζ� ��ȣ�� ������ ������:" << endl;
	for (int i = 0; i < 6; i++) {
		int n; cin >> n;
		if (!(1 <= n && n <= 45)) {
			cout << "1~45 ������� �մϴ�. �ٽ� �Է��ϼ���: ";
			i--;
		}
		else if (freq[n]) {
			cout << "��ȣ�� �ߺ��Ǿ����ϴ�. �ٽ� �Է��ϼ���: ";
			i--;
		}
		else {
			freq[n]++;
			_out_input[i] = n;
		}
	}
	cout << "�Է��� ��ȣ�Դϴ�: ";
	for (int i = 0; i < 6; i++)
		cout << _out_input[i] << " ";
	cout << endl;
}

void cpp07_generate_nums(int _out_nums[]) {
	//�ζ� ��ȣ ����
	srand(100);
	cout << "�ζ� ��ȣ�Դϴ�: ";
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
	//��ġ ���� ���
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 6; j++)
			if (_in_input[i] == _in_nums[j]) matches++;

	//��ũ ���
	int ranks[7] = { 0, 0, 0, 5, 4, 3, 1 };
	cout << "���� " << matches << "���� ��ġ�մϴ�";
	if (!ranks[matches])
		cout << ". " << "���Դϴ�... ������ �ٽ� �������ּ���." << endl;
	else
		cout << "! " << ranks[matches] << "���Դϴ�!" << endl;
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
		cout << "���ڸ� �Է��ϼ���: ";
		int input;
		cin >> input;
		if (input == -1)
			break;

		cout << input << "��(��) �Ҽ�";
		if (cpp07_check_prime_number(input))
			cout << "�Դϴ�.";
		else
			cout << "�� �ƴմϴ�.";
		cout << endl << endl;
	}

	cout << "���α׷��� �����մϴ�." << endl;
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
	cout << "�ڿ����� �Է��ϼ���: ";
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
	cout << "�ֻ����� ���� Ƚ���� �Է��ϼ���: ";
	int rolls; cin >> rolls;
	int freq[6] = { 0, };
	srand(100);
	for (int i = 0; i < rolls; i++)
		freq[rand() % 6]++;

	//ǥ�õǴ� ���� ������ �����Ѵ�.
	int show_chars = 64;
	for (int i = 0; i < 6; i++) {
		cout << (i + 1) << ":";
		cpp07_graph(show_chars, freq[i] * show_chars / rolls);
	}

	return 0;
}