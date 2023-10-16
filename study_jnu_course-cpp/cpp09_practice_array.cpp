#include <iostream>
using namespace std;


//1. Caesar cipher two
#include <string>
int cpp09_main1() {
	string str;

	printf("%d", 5);
	cout << "���ڿ��� �Է��ϼ���: ";
	getline(cin, str);
	
	int key;
	cout << "Ű�� �Է��ϼ���: ";
	cin >> key;

	for (int i = 0; i < str.length(); i++) {
		if (!('A' <= str[i] && str[i] <= 'Z'))
			continue;

		str[i] = (str[i] - 'A' + key + 26) % 26 + 'A';
	}

	cout << "cipher ���ڿ��Դϴ�: " << str << endl;

	return 0;
}


//2. Quite advanced calculator
int cpp09_main2() {
	int m1[3][3], m2[3][3];

	char op;
	cout << "������ �Է�: ";
	cin >> op;

	cout << "��� 1 �Է�: ";
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cin >> m1[i][j];

	cout << "��� 2 �Է�: ";
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cin >> m2[i][j];

	int ret[3][3] = { 0, };
	if (op == '+') {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				ret[i][j] = m1[i][j] + m2[i][j];
	}

	else if (op == '*') {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++)
					ret[i][j] += m1[i][k] * m2[k][j];
			}
	}

	cout << "Answer: " << endl;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			cout << ret[i][j] << " ";
		cout << endl;
	}


	return 0;
}


//3. Find a string
#include <string>
int cpp09_main3() {
	string str1, str2;
	cout << "string1: ";
	getline(cin, str1);
	cout << "string2: ";
	getline(cin, str2);

	int ret = -1;
	for (int i = 0; i < str1.length() - str2.length() + 1; i++) {
		int flag = 0;
		for (int j = 0; j < str2.length(); j++)
			if (str1[i + j] != str2[j])
				flag = 1;
		
		if (!flag) {
			ret = i;
			break;
		}
	}

	if (ret == -1)
		cout << "Not found" << endl;
	else
		cout << ret << endl;

	return 0;
}


//4. Binary Tree
#define cpp09_LEN_MAX 128
char cpp09_arr_btree[cpp09_LEN_MAX];

void cpp09_root(char key) {
	if (cpp09_arr_btree[1]) {
		cout << "��Ʈ�� �̹� �����Ǿ����ϴ�." << endl;
	}
	else {
		cpp09_arr_btree[1] = key;
		cout << "'" << key << "'��(��) ��Ʈ�Դϴ�." << endl;
	}
}

void cpp09_set_left(char key, int parentID) {
	if (2 * (parentID + 1) >= cpp09_LEN_MAX) {
		cout << "���� ������ ������ �ʰ��Ͽ����ϴ�." << endl;
	}
	else if (!cpp09_arr_btree[parentID + 1]) {
		cout << "�θ� �����Ǿ� ���� �ʽ��ϴ�." << endl;
	}	
	else if (cpp09_arr_btree[2 * (parentID + 1)]) {
		cout << "'" << cpp09_arr_btree[parentID + 1];
		cout << "'��(��) �̹� ���� �ڽ� ��尡 �����մϴ�." << endl;
	}
	else {
		cout << "'" << key << "'��(��) '" << cpp09_arr_btree[parentID + 1];
		cout << "' ���� �ڽ� ���� �߰��Ǿ����ϴ�." << endl;
		cpp09_arr_btree[2 * (parentID + 1)] = key;
	}
}

void cpp09_set_right(char key, int parentID) {
	if (2 * (parentID + 1) + 1 >= cpp09_LEN_MAX) {
		cout << "���� ������ ������ �ʰ��Ͽ����ϴ�." << endl;
	}
	else if (!cpp09_arr_btree[parentID + 1]) {
		cout << "�θ� �����Ǿ� ���� �ʽ��ϴ�." << endl;
	}
	else if (cpp09_arr_btree[2 * (parentID + 1) + 1]) {
		cout << "'" << cpp09_arr_btree[parentID + 1];
		cout << "'��(��) �̹� ������ �ڽ� ��尡 �ֽ��ϴ�." << endl;
	}
	else {
		cout << "'" << key << "'��(��) '" << cpp09_arr_btree[parentID + 1];
		cout << "' ������ �ڽ� ���� �߰��Ǿ����ϴ�." << endl;
		cpp09_arr_btree[2 * (parentID + 1) + 1] = key;
	}
}

void cpp09_print_tree_recursive(int parentID) {
	static int depth = -1;
	if (parentID >= cpp09_LEN_MAX) return;
	if (!cpp09_arr_btree[parentID]) return;

	depth++;
	for (int i = 0; i < depth * 2; i++)
		cout << " ";
	cout << cpp09_arr_btree[parentID] << "(" << (parentID - 1) << ")" << endl;
	cpp09_print_tree_recursive(2 * parentID);
	cpp09_print_tree_recursive(2 * parentID + 1);
	depth--;
}

void cpp09_print_tree() {
	if (!cpp09_arr_btree[1]) {
		cout << "Ʈ���� ������ϴ�." << endl;
		return;
	}

	cpp09_print_tree_recursive(1);
}

int cpp09_main4() {
	cpp09_root('A');
	cpp09_set_left('B', 0);
	cpp09_set_left('C', 0);
	cpp09_set_right('C', 0);
	cpp09_set_left('D', 1);
	cpp09_set_right('E', 3);
	cpp09_print_tree();

	return 0;
}