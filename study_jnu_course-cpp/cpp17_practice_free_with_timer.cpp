#include <iostream>
#include <thread>
#include <ctime>
using namespace std;

namespace cpp17_1 {

//1. Free the memory when the time is up!

template <typename T> class timer_smartptr {
private:
	T* ptr;
	thread t;

public:
	// thread funtion
	void timer(int time) {
		/* Write your code here :) */
		clock_t waiting = clock() + time * CLOCKS_PER_SEC;
		while (clock() < waiting); //time 초 동안 기다림

		if (ptr != nullptr) {
			delete ptr;
			cout << time << "초가 지나 메모리가 해제되었습니다 :P" << endl;
		}
	}

	timer_smartptr(T* p = 0, int time = 10) : ptr(p) {

		// set the thread
		t = thread(&timer_smartptr::timer, this, time);
	}
	~timer_smartptr() { }

	T& operator*() { return *ptr; }
	T* operator->() { return ptr; }

	void join() {
		t.join();
	}
};

int main() {
	//생성자가 호출되는 순간 새 쓰레드가 생성되면서,
	//그 쓰레드에서 timer(10)이 호출되어 실행된다!!
	timer_smartptr<int> p1(new int, 10); // e.g., 10s
	
	timer_smartptr<int> p2(new int, 3), p3(new int, 2), p4(new int, 7), p5(new int, 11);
	p1.join(); // wait till the time is up!
	cout << "p1.join() end" << endl;
	p2.join();
	cout << "p2.join() end" << endl;
	p3.join();
	cout << "p3.join() end" << endl;
	p4.join();
	cout << "p4.join() end" << endl;
	p5.join();
	cout << "p5.join() end" << endl;

	cout << "Wow!" << endl;
	return 0;
}


}

namespace cpp17_2 {


void func1() {
	for (int i = 0; i < 100; i++)
		cout << "I am thread 1(" << i << ")" << endl;
}

void func2() {
	for (int i = 0; i < 100; i++)
		cout << "I am thread 2(" << i << ")" << endl;
}

void func3() {
	for (int i = 0; i < 100; i++)
		cout << "I am thread 3(" << i << ")" << endl;
}



int main() {
	thread t1(func1);
	thread t2(func2);
	thread t3(func3);

	t1.join();
	t2.join();
	t3.join();

	return 0;
}

}


#include <cstdio>
#include <vector>
//#include <mutex>
namespace thread_advanced_1 {

void worker(vector<int>::iterator start, vector<int>::iterator end,
	int* result/*  , std::mutex& m  */) {
	int sum = 0;
	for (auto itr = start; itr < end; ++itr) {
		sum += *itr;
	}
	*result = sum;

	//m.lock();
	// 쓰레드의 id 를 구한다.
	thread::id this_id = std::this_thread::get_id();
	//printf("쓰레드 %x 에서 %d 부터 %d 까지 계산한 결과 : %d \n", this_id, *start, *(end - 1), sum);
	std::cout << "쓰레드 " << hex << this_id << " 에서 " << dec << *start << " 부터 "
		<< *(end - 1) << " 까지 계산한 결과 : " << sum << std::endl;
	//m.unlock();
}

int main() {
	vector<int> data(10000);
	for (int i = 0; i < 10000; i++) {
		data[i] = i;
	}

	// 각 쓰레드에서 계산된 부분 합들을 저장하는 벡터
	vector<int> partial_sums(4);

	//std::mutex m;
	vector<thread> workers;
	for (int i = 0; i < 4; i++) {
		workers.push_back(thread(worker, data.begin() + i * 2500,
			data.begin() + (i + 1) * 2500, &partial_sums[i]/*  , std::ref(m)  */));
	}

	for (int i = 0; i < 4; i++) {
		workers[i].join();
	}

	int total = 0;
	for (int i = 0; i < 4; i++) {
		total += partial_sums[i];
	}
	std::cout << "전체 합 : " << total << std::endl;

	return 0;
}

}


#include <mutex>
//mutex: mutual exclusion (상호 배제)
namespace thread_advanced_2 {

void worker(int& counter, std::mutex& m) {
	for (int i = 0; i < 10000; i++) {
		//m.lock();
		std::lock_guard<std::mutex> lock(m); //smart ptr 같은 느낌
		counter += 1;
		//m.unlock();
	}
}

int main() {
	int counter = 0;
	std::mutex m;

	vector<thread> workers;
	for (int i = 0; i < 4; i++) {
		// 레퍼런스로 전달하려면 ref 함수로 감싸야 한다 (지난 강좌 bind 함수 참조)
		workers.push_back(thread(worker, std::ref(counter), std::ref(m)));
	}

	for (int i = 0; i < 4; i++) {
		workers[i].join();
	}

	std::cout << "Counter 최종 값 : " << counter << std::endl;

	return 0;
}

}



namespace thread_deadlock_ex {

void worker1(std::mutex& m1, std::mutex& m2) {
	for (int i = 0; i < 10000; i++) {
		std::lock_guard<std::mutex> lock1(m1);
		std::lock_guard<std::mutex> lock2(m2);
		cout << "worker1()" << i << "\n";
		// Do something
	}
}

//해결법: lock 얻는 순서를 work1과 같게 한다
//또는 lock1을 try해본 다음, lock이 실패하면 lock2를 언락해 다른 스레드에게 양보한다.
void worker2(std::mutex& m1, std::mutex& m2) {
	for (int i = 0; i < 10000; i++) {
		std::lock_guard<std::mutex> lock2(m2);
		std::lock_guard<std::mutex> lock1(m1);
		cout << "worker2()" << i << "\n";
		// Do something
	}
}

int main() {
	int counter = 0;
	std::mutex m1, m2;  // 우리의 mutex 객체

	std::thread t1(worker1, std::ref(m1), std::ref(m2));
	std::thread t2(worker2, std::ref(m1), std::ref(m2));

	t1.join();
	t2.join();

	std::cout << "끝!" << std::endl;

	return 0;
}

}


//int main() { thread_deadlock_ex::main(); }