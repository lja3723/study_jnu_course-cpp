#include <iostream>
using namespace std;
#define FN static void

namespace CH08_namespace {
	class Car {
	private:
		//int speed = 100; //모던 C++에서 가능함 (비추천)
		int speed;
		int gear;

		const int cons;
		int& ref;
	public:
		//Initialization list: 초기화 리스트
		Car() : cons(0), ref(speed) {
			cout << "생성자 Car() 호출" << endl;
			speed = 0;
			gear = 0;
			//cons = 10;	선언후 대입과 동일; 불가능
			//ref = &speed;	마찬가지로 불가능
		}
		Car(int p) : cons(0), ref(speed) {
			cout << "생성자 Car(int) 호출" << endl;
			speed = p;
			gear = p;
		}
		//파괴자
		~Car() {
			cout << "파괴자 ~Car() 호출" << endl;
		}
		int accel() {
			speed += 10;
			return speed;
		}
		int brake() {
			speed -= 10;
			return speed;
		}
	};
}

using namespace CH08_namespace;

class CH08 {
public:
	FN initializer() {
		//Car myCar = { 10, 0 }; #private 때문에 불가능
		// Car()			기본 생성자
		// Car(const Car &)	복사 생성자
		// Car(Car &&)		이동 생성자
		// Car(int k)		변환 생성자

		Car myCar = Car();
		myCar.accel();
		myCar.accel();
		cout << myCar.accel() << endl;

		Car myCar2(100);
		cout << myCar.accel() << endl;
	}
};