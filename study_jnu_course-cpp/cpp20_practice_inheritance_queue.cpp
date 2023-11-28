#include <iostream>
#include <vector>
using namespace std;

namespace cpp20 {
	//1. Queue version 2.

	class my_queue : private vector<int> {
	public:
		void enqueue(int e) { insert(begin(), e); }
		int peek() { return back(); }
		void dequeue() { pop_back(); }
	};

	int main() {
		my_queue q;

		q.enqueue(10);
		q.enqueue(20);
		q.enqueue(30);
		q.enqueue(40);

		cout << q.peek() << endl; // 10
		q.dequeue();
		cout << q.peek() << endl; // 20
		q.dequeue();
		cout << q.peek() << endl; // 30
		q.dequeue();
		cout << q.peek() << endl; // 40
		q.dequeue();

		return 0;
	}
}

//int main() { cpp20::main(); }