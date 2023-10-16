#include <iostream>
using namespace std;


//1. Caesar cipher two
#include <string>
int cpp09_main1() {
	string str;

	printf("%d", 5);
	cout << "문자열을 입력하세요: ";
	getline(cin, str);
	
	int key;
	cout << "키를 입력하세요: ";
	cin >> key;

	for (int i = 0; i < str.length(); i++) {
		if (!('A' <= str[i] && str[i] <= 'Z'))
			continue;

		str[i] = (str[i] - 'A' + key + 26) % 26 + 'A';
	}

	cout << "cipher 문자열입니다: " << str << endl;

	return 0;
}


//2. Quite advanced calculator
int cpp09_main2() {
	int m1[3][3], m2[3][3];

	char op;
	cout << "연산자 입력: ";
	cin >> op;

	cout << "행렬 1 입력: ";
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cin >> m1[i][j];

	cout << "행렬 2 입력: ";
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
		cout << "루트가 이미 설정되었습니다." << endl;
	}
	else {
		cpp09_arr_btree[1] = key;
		cout << "'" << key << "'이(가) 루트입니다." << endl;
	}
}

void cpp09_set_left(char key, int parentID) {
	if (2 * (parentID + 1) >= cpp09_LEN_MAX) {
		cout << "설정 가능한 범위를 초과하였습니다." << endl;
	}
	else if (!cpp09_arr_btree[parentID + 1]) {
		cout << "부모가 설정되어 있지 않습니다." << endl;
	}	
	else if (cpp09_arr_btree[2 * (parentID + 1)]) {
		cout << "'" << cpp09_arr_btree[parentID + 1];
		cout << "'은(는) 이미 왼쪽 자식 노드가 존재합니다." << endl;
	}
	else {
		cout << "'" << key << "'이(가) '" << cpp09_arr_btree[parentID + 1];
		cout << "' 왼쪽 자식 노드로 추가되었습니다." << endl;
		cpp09_arr_btree[2 * (parentID + 1)] = key;
	}
}

void cpp09_set_right(char key, int parentID) {
	if (2 * (parentID + 1) + 1 >= cpp09_LEN_MAX) {
		cout << "설정 가능한 범위를 초과하였습니다." << endl;
	}
	else if (!cpp09_arr_btree[parentID + 1]) {
		cout << "부모가 설정되어 있지 않습니다." << endl;
	}
	else if (cpp09_arr_btree[2 * (parentID + 1) + 1]) {
		cout << "'" << cpp09_arr_btree[parentID + 1];
		cout << "'은(는) 이미 오른쪽 자식 노드가 있습니다." << endl;
	}
	else {
		cout << "'" << key << "'이(가) '" << cpp09_arr_btree[parentID + 1];
		cout << "' 오른쪽 자식 노드로 추가되었습니다." << endl;
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
		cout << "트리가 비었습니다." << endl;
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