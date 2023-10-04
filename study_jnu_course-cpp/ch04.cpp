#include <iostream>
#include <ctime>
using namespace std;
#define FN static void

#include <random>

class CH04 {
public:
	FN if_statement() {
		int num = 10;
		if (true)	cout << "1" << endl;
		if (1)		cout << "2" << endl;
		if (-1)		cout << "3" << endl;
		if ('a')	cout << "4" << endl;
		if (num)	cout << "5" << endl;
		if (0)	cout << "6" << endl;
	}
	FN range_check() {
		int a = 15;

		if (5 < a <= 10) //NOT
			cout << "a는 5보다 크고 10보다 크거나 작다.\n";
		else
			cout << "else\n";

		if (5 < a && a <= 10) //OK
			cout << "a는 5보다 크고 10보다 크거나 작다.\n";
		else
			cout << "else\n";
	}
	FN logic_op() {
		int a = 10, b = 20;
		cout << "a && b: " << (a && b) << endl;
		cout << "a || b: " << (a || b) << endl;
		cout << "!a: " << (!a) << endl;

		//최근에 추가된 기능
		cout << "a and b: " << (a and b) << endl;
		cout << "a or b: " << (a or b) << endl;
		cout << "not a: " << (not a) << endl;
	}
	FN if_statement_2() {
		int grade = 87;
		
		if (grade >= 80)
			if (grade >= 90)
				cout << "학점은 A입니다!" << endl;

			else if (grade >= 85)
				cout << "학점은 B+입니다!" << endl;
			else
				cout << "학점은 B입니다!" << endl;
		else
			cout << "학점은 C입니다!" << endl;
	}
	FN switch_advanced() {
		//switch 판별식에는 무조건 정수만 들어갈 수 있음
		char credit;
		cin >> credit;
		switch (credit) {
		case 'a' || 'A': // case 1: 과 동일함!
			cout << "case 'a' || 'A'" << endl;
			break;			
		}
		
		char dredit;
		cin >> dredit;
		switch (credit || dredit) { //0 또는 1밖에 안됨
		case 0:
			cout << "credit || dredit == 0" << endl;
			break;
		case 1:
			cout << "credit || dredit == 1" << endl;
			break;
		}

		float fredit;
		/*	스위치 안에 실수 표현 들어가기 불가능
		switch (fredit) {
		case 3.5:
			break;
		}
		*/
	}
	FN loop_usage() {
		int a;
		clock_t start;

		int simul = 100;
		double if_t = 0;
		double switch_t = 0;
		double t = 0;

		for (int i = 0; i < simul; i++) {
			a = 5, start = clock();
			for (int i = 0; i < 1e8; i++) {
				if (a < 10)
					a = 15;
				else if (a < 20)
					a = 25;
				else if (a < 30)
					a = 35;
				else
					a = 5;
			}
			t = (clock() - start) / 1000.0;
			cout << "if문 시간: " << t << "초\n";
			if_t += t;

			a = 5, start = clock();
			for (int i = 0; i < 1e8; i++) {
				switch (a) {
				case 5:
					a = 15;
					break;
				case 15:
					a = 25;
					break;
				case 25:
					a = 35;
					break;
				default:
					a = 5;
				}
			}
			t = (clock() - start) / 1000.0;
			cout << "switch문 시간: " << t << "초\n";
			switch_t += t;
		}

		cout << "평균 시간(초):" << endl;
		cout << "if문: " << if_t / simul << endl;
		cout << "switch문: " << switch_t / simul << endl;
		cout << "Launch! " << endl;
	}
	FN random_ex() {
		//#include <random> 라이브러리 필요
		random_device rd;
		mt19937 gen{ rd() };

		uniform_int_distribution<> dist(1, 45);

		for (int i = 0; i < 100; i++) {
			//cout << (gen() % 45) + 1 < endl;
			cout << gen() << endl;
		}
	}
	FN goto_statement() {

		cout << "Bye!\n";
		goto hello;

		cout << "Hello?\n";

	hello:

		cout << "end!\n";
	}
};