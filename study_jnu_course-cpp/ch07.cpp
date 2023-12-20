#include <iostream>
#include <ctime>
using namespace std;
#define FN static void



class Add {
public:
	int x;
	Add(int a) : x(a) { }
	Add operator+(int k) {
		return Add(x + k);
	}
};

class CH07 {
	struct Big {
		int a;
		int b[1000];
		long c;
		int* d;
	};


	FN deliver(int* num) {
		cout << num << endl;
		cout << *num << endl;
	}
	FN array_param_test(int* num) {
		int i = 100;
		num = &i;
	}
	static int* return_array() {
		int arr[] = { 5, 10, 15, 20 };
		return arr;
	}
	FN junk_func() {
		int a = -10;
		int b = -20;
		int c = -30;
		int d = -40;
	}
	static int* dyalloc() {
		int* num = new int[4]; //heap 영역에 메모리 할당받고 그 주소 반환
		num[0] = 10, num[1] = 20;
		num[2] = 30, num[3] = 40;
		return num;
	}
	FN deliver_struct_test(Big value) {
		cout << sizeof(value) << endl;
		cout << value.a << endl;
	}
	FN deliver_struct_pointer_test(Big* value) {
		cout << sizeof(value) << endl;
		cout << value->a << endl;    // (1)
		cout << (*value).a << endl;  //  (2)
		//(1) == (2)
		cout << *value->d << endl;   // (3)
		cout << *(value->d) << endl; // (4)
		//(3) == (4)
	}
	static double func_for_ptr1(int k, double t) {
		cout << "func_for_ptr1" << endl;
		return k * t;
	}
	static int func_for_ptr2() { 
		cout << "func_for_ptr2" << endl; 
		return 2;
	}
	static int func_for_ptr3() {
		cout << "func_for_ptr3" << endl;
		return 3;
	}
	static int func_for_ptr4() {
		cout << "func_for_ptr4" << endl;
		return 4;
	}
	static double time_measure(void (*ptr)()) {
		clock_t start, end;
		start = clock();
		for (int i = 0; i < 1000000; i++) ptr();
		end = clock();
		return (double)(end - start) / CLOCKS_PER_SEC;
	}
	static void timefunc1() { for (int i = 0; i < 100; i++); }
	static void timefunc2() { for (int i = 0; i < 1000; i++); }
	//함수 포인터를 반환하는 함수
	static double (*ret_fptr(int k))(int, double) {
		cout << k * k << endl;
		return func_for_ptr1;
	}
	FN no_array_decay_with_ref(int(&ref)[4]) {
		cout << sizeof(ref) << endl;
		cout << ref[3] << endl;
	}

public:
	FN pointer_intro() {
		int i = 10, j = 20, k = 30;
		int* ptr = &i;
		*ptr += 50;
		cout << ptr << endl << i << endl;
		ptr = (int*)100;
		cout << ptr << endl;

		int** dptr = &ptr;
		*dptr = &j;
		*ptr *= 6;
		cout << ptr << endl << j << endl;
		cout << &i << endl << &j << endl << &k << endl;
		cout << (&i - &i) << endl << (&j - &i) << endl << (&k - &i) << endl;
	}
	/////// fill here ///////
	FN stack_frame_ex() {
		int i = 10;
		int j = 20;
		int k = 30;
		int* ptr = &i;

		int x = 40;
		int y = 50;
		int z = 60;
		int* qtr = &i;

		cout << &i << endl;
		cout << &j << endl;
		cout << &k << endl;
		cout << &x << endl;
		cout << &y << endl;
		cout << &z << endl;
	}
	FN stack_frame_ex2() {
		int a = 1;
		int b = 2;
		int c = 3;
		cout << &a << endl;
		cout << &b << endl;
		cout << &c << endl;

		cout << &a << endl;
		cout << (&a + 1) << endl;
	}
	FN stack_frame_ex3() {
		long long l = 0x8899aabbccddeeff;
		char* ptr = (char*)&l;

		cout << hex;
		cout << (void*)ptr << endl;
		cout << (void*)(ptr + 4) << endl;
		cout << ((int*)(ptr + 3)) << endl;
	}
	FN stack_frame_ex4() {
		long long l = 0x8899aabbccddeeff;
		int* ptr = (int*)&l;

		cout << hex;
		cout << l << endl;
		cout << ptr[0] << endl;
		cout << ptr[1] << endl;
		cout << *((short*)((char*)ptr + 5)) << endl;
	}
	
	FN pointer_usage_array_deliver() {
		int num[] = { 10, 20, 30, 40 };
		int i = 50;
		cout << num << endl;
		deliver(num);
	}
	FN const_array() {
		int num[] = { 1, 2, 3, 4 }; // == int * const num;
		int i = 100;
		//num = &i; //error
	}
	FN get_array_test() {
		int* ptr;
		ptr = return_array();
		for (int i = 0; i < 4; i++) {
			cout << ptr[i] << endl;
		}
		junk_func();
		for (int i = 0; i < 4; i++) {
			cout << ptr[i] << endl; 
			//의미없는 쓰레기 값 참조!
			//오염된 스택을 읽어온다.
		}
	}
	FN dynamic_alloc_intro() {
		int* ptr;
		ptr = dyalloc();
		cout << *ptr << endl;
		junk_func();
		cout << *ptr << endl;
		delete[4] ptr;

		//int arr[1000000]; 대신 int* arr = new int[1000000];
	}
	FN memory_leak_and_sol() {
		int* ptr = new int;
		*ptr = 10;
		ptr = new int; //이전에 참조된 메모리가 leak!
		cout << ptr << endl;
		delete ptr;    //ptr이 지워진게 아니고 ptr이 가리키는 메모리를 반환
		cout << ptr << endl; //delete되어도 ptr 저장값은 변화 x
		*ptr = 500; //dangling pointer (free된 메모리 참조!)
		cout << *ptr << endl << endl;

		//dangling pointer 피하는법
		//하나의 동적변수를 여러 포인터로 참조하는 것 피하기
		//delete되면 포인터를 0/nullptr로 초기화
		ptr = new int;
		int* qtr = ptr;
		*qtr = 500;
		delete qtr;
		cout << *ptr << endl; //dangling pointer 이에요!
		ptr = qtr = nullptr; // delete 후에는 이런 처리가 필요
		//nullptr은 하나의 클래스이다
		cout << *ptr << endl;
		
	}
	FN deliver_structure() {
		Big mystruct;
		mystruct.a = 100;
		mystruct.d = &mystruct.a;
		*mystruct.d = 500; // *(mystruct.d) 와 동일
		//cout << sizeof(mystruct) << endl;
		//deliver_struct_test(mystruct);
		deliver_struct_pointer_test(&mystruct);
	}
	FN func_ptr() {
		cout << func_for_ptr1 << endl;
		cout << (void*)&func_for_ptr2 << endl;
		cout << (void*)&func_for_ptr3 << endl;
		cout << (void*)&func_for_ptr4 << endl;

		//함수 포인터!!
		double (*ptr)(int, double) = func_for_ptr1;
		cout << ptr(5, 3.1) << endl;
		cout << (*ptr)(5, 2.4) << endl;

		//함수 포인터는 함수를 변수처럼 쓰게 해 줘요!!
		//아래는 함수 포인터의 포인터임
		double (**dfptr)(int, double) = &ptr;
		cout << (*dfptr)(5, 2.1) << endl;

	}
	FN func_ptr_to_param() {
		cout << time_measure(timefunc1) << endl;
		cout << time_measure(timefunc2) << endl;
	}
	FN ret_func_ptr() {
		cout << ret_fptr(12)(6, 1.4) << endl;
	}
	FN ptr_and_const_test() {
		int a = 100, b = 10;
		// 1) ptr이 가리키는 그 원소는 const
		const int* ptr1 = &a;
		//*ptr1 = 500; 불가능
		ptr1 = &b;

		// 2) 변수 ptr은 const (주소 변경 불가)
		int* const ptr2 = &a;
		*ptr2 = 50;
		//ptr2 = &b; 불가능

		// 3) (1)과 (2)가 모두 적용
		const int* const ptr3 = &a;
		//*ptr3 = 500; 불가능
		//ptr3 = &b; 불가능
		
	}
	FN avoid_decay_using_ref() {
		int arr[4] = { 10, 20, 30, 40 };
		no_array_decay_with_ref(arr);
	}
	
	FN test() {
		Add a(10);
		a = a.operator+(20) + 30;
		cout << a.x << endl;
	}

};


//int main() { CH07::test(); }