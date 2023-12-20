#include <iostream>
#include <string>
#include <vector>
#include <conio.h>

#include <functional>
#include <queue>
#include <algorithm>
#include <ctime>
using namespace std;

namespace cpp21 {
	//1. Build your own menu!

	#define clrscr() system("cls");
	#define getkey() _getch();


	class AbstractMenu {
		string title;
	public:
		AbstractMenu(string s) : title(s) {}
		virtual ~AbstractMenu() {}
		string getTitle() { return title; }
		virtual void command() = 0;
	};


	class PopupMenu : public AbstractMenu {
		vector<AbstractMenu*> v;
	public:
		PopupMenu(string s) : AbstractMenu(s) {}
		~PopupMenu() {
			for (int i = 0; i < v.size(); i++)
				delete v[i];
		}

		void addMenu(AbstractMenu* p) { v.push_back(p); }

		virtual void command() {
			while (true) {
				clrscr();

				int size = v.size();
				for (int i = 0; i < size; i++) 
					cout << i + 1 << ". " << v[i]->getTitle() << endl;
				cout << size + 1 << ". << Back" << endl;
				
				cout << "Select an item >> ";
				int cmd;
				cin >> cmd;

				if (cmd == size + 1) break;
				if (cmd < 1 || cmd > size + 1) continue;

				v[cmd - 1]->command();
			}
		}
	};


	class ContainerCmd : public AbstractMenu {
		deque<int>& cont;
		function<void(deque<int>&)> cmd;
	public:
		ContainerCmd(string s, deque<int>& _cont, function<void(deque<int>&)> _cmd)
			: AbstractMenu(s), cont(_cont), cmd(_cmd) { }
		virtual void command() { cmd(cont); }
	};


	void show_contents(deque<int>& c) {
		cout << "elements of container(size = " << c.size() << "):" << endl << "  ";
		for (auto& e : c) cout << e << ";  ";
		cout << (c.empty() ? "[empty]" : "\n");
		getkey();
	}


	ContainerCmd* item_show_menu(deque<int>& cont) {
		return new ContainerCmd("Show contents", cont, show_contents);
	}


	int main() {
		cout << "***  Container Management System  ***\n\n";
		cout << "This program allows managing a container as a stack or queue." << endl;
		cout << "And also supports some commands for container." << endl;
		cout << "You can select options by input a number." << endl;
		cout << "Press any key to continue...";
		getkey();

		//container
		deque<int> cont;		

		//Root menubar
		PopupMenu* root = new PopupMenu("ROOT MENUBAR");

		//Sub-menus
		PopupMenu* menu_actions = new PopupMenu("Show actions");
		PopupMenu* menu_treat_st = new PopupMenu("Treat as a stack");
		PopupMenu* menu_treat_q = new PopupMenu("Treat as a queue");


		//menus for "container actions"
		PopupMenu* menu_rearrange = new PopupMenu("Rearrange elements");
		ContainerCmd* cmd_sort_ascending = new ContainerCmd(
			"Sorting elements by ascending order", cont, [](deque<int>& c) {
				sort(c.begin(), c.end());
				cout << "Sorting by ascending order has been done." << endl;
				show_contents(c);
			});
		ContainerCmd* cmd_sort_descending = new ContainerCmd(
			"Sorting elements by descending order", cont, [](deque<int>& c) {
				sort(c.begin(), c.end(), greater<int>());
				cout << "Sorting by descending order has been done." << endl;
				show_contents(c);
			});
		//set random seed for shuffle
		srand((unsigned)time(NULL));
		ContainerCmd* cmd_shuffle = new ContainerCmd(
			"Shuffle elements", cont, [](deque<int>& c) {
				for (int i = 0; i < c.size(); i++) {
					int j = rand() % c.size();
					int tmp = c[i];
					c[i] = c[j];
					c[j] = tmp;
				}
				cout << "Shuffle elements has been done." << endl;
				show_contents(c);
			});


		PopupMenu* menu_arithmetic = new PopupMenu("Arithmetic Operations");
		ContainerCmd* cmd_add_scala = new ContainerCmd(
			"Add a scala", cont, [](deque<int>& c) {
				cout << "Enter an integer scala to add: ";
				int scala; cin >> scala;
				for (auto& e : c) e += scala;
				cout << "Operation has been done." << endl;
				show_contents(c);
			});
		ContainerCmd* cmd_multiply_scala = new ContainerCmd(
			"Multiply a scala", cont, [](deque<int>& c) {
				cout << "Enter a real number scala to multiply: ";
				double scala; cin >> scala;
				for (auto& e : c) e *= scala;
				cout << "Operation has been done." << endl;
				show_contents(c);
			});
		ContainerCmd* cmd_accumulate = new ContainerCmd(
			"Accumulate a scala", cont, [](deque<int>& c) {
				if (c.empty()) {
					cout << "Operation failed because container was empty!" << endl;
					getkey();
					return;
				}
				int ret = 0;
				for (auto& e : c) ret += e;
				cout << "Accumulated value is " << ret << "." << endl;
				show_contents(c);
			});


		ContainerCmd* cmd_find = new ContainerCmd(
			"Find an element", cont, [](deque<int>& c) {
				cout << "Enter a number to find: ";
				int to_find; cin >> to_find;
				auto it = find(c.begin(), c.end(), to_find);
				if (it != c.end()) {
					cout << "Found value " << to_find << "." << endl;
					cout << "index = " << int(it - c.begin()) << endl;
				}
				else {
					cout << "Value " << to_find << " not found." << endl;
				}
				getkey();
			});
		ContainerCmd* cmd_remove_all_if = new ContainerCmd(
			"Remove all matched elements", cont, [](deque<int>& c) {
				cout << "Enter a number to remove: ";
				int to_remove; cin >> to_remove;
				c.erase(
					remove_if(
						c.begin(), c.end(), 
						[to_remove](int& e) { return e == to_remove; }), 
					c.end()
				);
				cout << "Operation has been done." << endl;
				show_contents(c);
			});
		ContainerCmd* cmd_clear = new ContainerCmd(
			"Clear all elements", cont, [](deque<int>& c) {
				c.clear();
				cout << "All elements of container cleared." << endl;
				getkey();
			});



		//menus for "treat as stack menu"
		ContainerCmd* cmd_st_push = new ContainerCmd(
			"Push an element", cont, [](deque<int>& c) {
				cout << "Enter an element to push: ";
				int ret; cin >> ret;
				c.push_back(ret);
				cout << "Push successed." << endl;
				show_contents(c);
			});
		ContainerCmd* cmd_st_pop = new ContainerCmd(
			"Pop an element", cont, [](deque<int>& c) {
				if (c.empty()) {
					cout << "Cannot pop an element because container was empty!" << endl;
					getkey();
					return;
				}
				int pop = c.back();
				c.pop_back();
				cout << "Pop successed. poped element: " << pop << endl;
				show_contents(c);
			});


		//menus for "treat as queue menu"
		ContainerCmd* cmd_q_push = new ContainerCmd(
			"Enqueue an element", cont, [](deque<int>& c) {
				cout << "Enter an element to enqueue: ";
				int ret; cin >> ret;
				c.push_back(ret);
				cout << "Enqueue successed." << endl;
				show_contents(c);
			});
		ContainerCmd* cmd_q_pop = new ContainerCmd(
			"Dequeue an element", cont, [](deque<int>& c) {
				if (c.empty()) {
					cout << "Cannot dequeue an element because container was empty!" << endl;
					getkey();
					return;
				}
				int pop = c.front();
				c.pop_front();
				cout << "Dnqueue successed. Dequeued element: " << pop << endl;
				show_contents(c);
			});


		//Append menus and cmds recursively
		root->addMenu(item_show_menu(cont)); //Ŀ�ǵ�: �����̳� ��ü ����

		root->addMenu(menu_actions); //�޴�: �����̳ʿ� ���� Ư���� ���� ����
		menu_actions->addMenu(item_show_menu(cont)); //Ŀ�ǵ�: �����̳� ��ü ����

		//*****************************************************
		menu_actions->addMenu(menu_rearrange); //�޴�: ���� ��迭�ϱ�
		menu_rearrange->addMenu(item_show_menu(cont)); //Ŀ�ǵ�: �����̳� ��ü ����
		menu_rearrange->addMenu(cmd_sort_ascending); //Ŀ�ǵ�: �������� �����ϱ�
		menu_rearrange->addMenu(cmd_sort_descending); //Ŀ�ǵ�: �������� �����ϱ�
		menu_rearrange->addMenu(cmd_shuffle); //Ŀ�ǵ�: ������ �迭�ϱ�

		//*****************************************************
		menu_actions->addMenu(menu_arithmetic); //�޴�: �����̳ʿ� ���� ������� �����ϱ�
		menu_arithmetic->addMenu(item_show_menu(cont)); //Ŀ�ǵ�: �����̳� ��ü ����
		menu_arithmetic->addMenu(cmd_add_scala); //Ŀ�ǵ�: ��� ���ҿ� ���� ��Į�� ���ϱ�
		menu_arithmetic->addMenu(cmd_multiply_scala); //Ŀ�ǵ�: ��� ���ҿ� �Ǽ� ��Į�� ���ϱ�
		menu_arithmetic->addMenu(cmd_accumulate); //Ŀ�ǵ�: ��� ���Ҹ� ���� �� Ȯ���ϱ�

		//*****************************************************
		menu_actions->addMenu(cmd_find); //Ŀ�ǵ�: ���� ã��
		menu_actions->addMenu(cmd_remove_all_if); //Ŀ�ǵ�: ��ġ�ϴ� ���� ��� �����ϱ�
		menu_actions->addMenu(cmd_clear); //Ŀ�ǵ�: �����̳� ����

		//*****************************************************
		root->addMenu(menu_treat_st); //�޴�: �����̳ʸ� �������� �ٷ��
		menu_treat_st->addMenu(item_show_menu(cont)); //Ŀ�ǵ�: �����̳� ��ü ����
		menu_treat_st->addMenu(cmd_st_push); //Ŀ�ǵ�: push an element
		menu_treat_st->addMenu(cmd_st_pop); //Ŀ�ǵ�: pop an element

		//*****************************************************
		root->addMenu(menu_treat_q); //�޴�: �����̳ʸ� ť�� �ٷ��
		menu_treat_q->addMenu(item_show_menu(cont)); //Ŀ�ǵ�: �����̳� ��ü ����
		menu_treat_q->addMenu(cmd_q_push); //Ŀ�ǵ�: push an element
		menu_treat_q->addMenu(cmd_q_pop); //Ŀ�ǵ�: pop an element

		//Launch by call the root command!
		root->command();

		cout << "The Program has been terminated." << endl;

		delete root;

		return 0;
	}
}

//int main() { cpp21::main(); }