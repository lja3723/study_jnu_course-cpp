//�а�: ����Ʈ������а�
//�й�: 201986
//�̸�: �����
//����: cpp02_practice(cin,cout)

#include <iostream>
using namespace std;

void cpp02_practice() {
	cout << endl << endl << "[ 1. Simple calculator ]" << endl;
	int input1, input2;
	cout << "�Է°� 1: ";
	cin >> input1;
	cout << "�Է°� 2: ";
	cin >> input2;
	cout << "���� ���: " << input1 + input2 << endl;
	cout << "���� ���: " << input1 - input2 << endl;
	cout << "���� ���: " << input1 * input2 << endl;
	cout << "������ ���: " << input1 / input2 << endl;


	cout << endl << endl << "[ 2. ID card (?) ]" << endl;
	int student_id;
	string name;
	cout << "�й� �Է�: ";
	cin >> student_id;
	cout << "�̸� �Է�: ";
	cin >> name;
	cout << "�� �̸��� " << name << "�Դϴ�." << endl;
	cout << "�� �й��� " << student_id << "�Դϴ�." << endl;
	cout << "�� �̸��� �й��� " << name << ", " << student_id << "�Դϴ�." << endl;


	cout << endl << endl << "[ 3. Wrong number ]" << endl;
	int value;
	cout << "���� �Է�: ";
	cin >> value;
	cout << "�Է��� ��: " << value;
	/* Answer:
	*	���α׷��� ���� ������� �ʰ� ������ �ȴ�.
	*	���� �Է� ���ڿ��� ���ڷ� �������� �ʾҴٸ�
	*	int �������� 0�� ����Ǹ�, ���ڷ� �����ߴٸ�
	*	���ʷ� ���ڰ� �ƴ� ���ڰ� �����ϱ� �������� ���ڰ�
	*	������ ����ȴ�.
	*	���� �����ϴ� ��� cin ������ �Է��� ���� �ʰ�
	*	������������ �۵��ȴ�.
	*/


	cout << endl << endl << "[ 4. How old are you? ]" << endl;
	cout << "����� ���̸� �Է��ϼ���: ";
	int age;
	cin >> age;
	cout << "���̸� �������� ��ȯ�մϴ�: " << age * 12;


	cout << endl << endl << "[ 5. Celsius and Fahrenheit ]" << endl;
	cout << "���� �µ� ���� �Է��ϼ���: ";
	int celsius;
	cin >> celsius;
	cout << "����" << celsius << "���� ȭ�� " << 1.8 * celsius + 32 << "�� �Դϴ�." << endl;
}