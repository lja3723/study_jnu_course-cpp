//학과: 소프트웨어공학과
//학번: 201986
//이름: 이장안
//제목: cpp02_practice(cin,cout)

#include <iostream>
using namespace std;

void cpp02_practice() {
	cout << endl << endl << "[ 1. Simple calculator ]" << endl;
	int input1, input2;
	cout << "입력값 1: ";
	cin >> input1;
	cout << "입력값 2: ";
	cin >> input2;
	cout << "덧셈 결과: " << input1 + input2 << endl;
	cout << "뺄셈 결과: " << input1 - input2 << endl;
	cout << "곱셈 결과: " << input1 * input2 << endl;
	cout << "나눗셈 결과: " << input1 / input2 << endl;


	cout << endl << endl << "[ 2. ID card (?) ]" << endl;
	int student_id;
	string name;
	cout << "학번 입력: ";
	cin >> student_id;
	cout << "이름 입력: ";
	cin >> name;
	cout << "제 이름은 " << name << "입니다." << endl;
	cout << "제 학번은 " << student_id << "입니다." << endl;
	cout << "제 이름과 학번은 " << name << ", " << student_id << "입니다." << endl;


	cout << endl << endl << "[ 3. Wrong number ]" << endl;
	int value;
	cout << "정수 입력: ";
	cin >> value;
	cout << "입력한 값: " << value;
	/* Answer:
	*	프로그램이 강제 종료되지 않고 실행이 된다.
	*	만약 입력 문자열이 숫자로 시작하지 않았다면
	*	int 변수에는 0이 저장되며, 숫자로 시작했다면
	*	최초로 숫자가 아닌 문자가 등장하기 전까지의 숫자가
	*	변수에 저장된다.
	*	이후 등장하는 모든 cin 구문은 입력을 받지 않고
	*	비정상적으로 작동된다.
	*/


	cout << endl << endl << "[ 4. How old are you? ]" << endl;
	cout << "당신의 나이를 입력하세요: ";
	int age;
	cin >> age;
	cout << "나이를 개월수로 변환합니다: " << age * 12;


	cout << endl << endl << "[ 5. Celsius and Fahrenheit ]" << endl;
	cout << "섭씨 온도 값을 입력하세요: ";
	int celsius;
	cin >> celsius;
	cout << "섭씨" << celsius << "도는 화씨 " << 1.8 * celsius + 32 << "도 입니다." << endl;
}