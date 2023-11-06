#include <iostream>
using namespace std;

namespace cpp14_1 {
//1. Calculator with a class

class my_calc {
private:
	double a, b;
public:
	my_calc(double x, double y) : a(x), b(y) {}
	double add() { return a + b; }
	double sub() { return a - b; }
	double mul() { return a * b; }
	double div() {
		if (b == 0) {
			cout << "0���� ���� �� �����ϴ�." << endl;
			return 0;
		}
		return a / b;
	}
};

int main() {
	my_calc c(10, 20);

	cout << c.add() << endl;
	cout << c.sub() << endl;
	cout << c.mul() << endl;
	cout << c.div() << endl;

	return 0;
}
/********end********/}
namespace cpp14_2{
	//2. Queue

template <typename V>
class my_queue {
private:
	struct Node {
		V data;
		Node* prev;
		Node* next;
	};
	Node* head, * tail;
	int size;
	V invalid_value() {
		V value = (V)0;
		memset(&value, 0, sizeof(V));
		return value;
	}
public:
	my_queue() {
		head = tail = nullptr;
		size = 0;
	}
	~my_queue() {
		while (head != nullptr) {
			Node* delNode = head;
			head = head->next;
			delete delNode;
		}
	}
	void Enqueue(V value) {
		Node* newNode = new Node;
		newNode->data = value;
		size++;

		if (head == nullptr) {
			head = tail = newNode;
			newNode->prev = newNode->next = nullptr;
			return;
		}

		newNode->next = head;
		newNode->prev = nullptr;
		newNode->next->prev = head = newNode;
	}
	V Dequeue() {
		if (tail == nullptr) {
			cout << "[error; unable to Dequeue(): ť�� �������] ";
			return invalid_value();
		}

		Node* delNode = tail;
		V ret = delNode->data;
		if (tail->prev != nullptr)
			tail->prev->next = nullptr;
		tail = tail->prev;
		if (tail == nullptr)
			head = nullptr;

		delete delNode;
		size--;
		return ret;
	}
	bool empty() {
		return size == 0;
	}
	V Peek() {
		if (tail == nullptr) {
			cout << "[error; unable to Peek(): ť�� �������] ";
			return invalid_value();
		}
		return tail->data;
	}
};

int main() {
	my_queue<int> q;

	cout << "������� �� Dequeue() �õ�: " << endl;
	cout << q.Dequeue() << endl;
		
	cout << "Enqueue() 6�� ����" << endl;
	q.Enqueue(5); q.Enqueue(10); q.Enqueue(-4);
	q.Enqueue(7); q.Enqueue(8); q.Enqueue(-1);
		
	cout << "Peek() ����:" << q.Peek() << endl;

	cout << "ť�� �������� Dequeue() ����:" << endl;
	while (!q.empty()) {
		cout << "Dequeue(): " << q.Dequeue() << endl;
	}
		
	cout << "������� �� Peek(), Dequeue() ����" << endl;
	cout << q.Peek() << endl;
	cout << q.Dequeue() << endl;

	q.Enqueue(1004); q.Enqueue(-30); q.Enqueue(48);
	//�ı��ڿ� ���� ���� delete �����
	return 0;
}
/********end********/}