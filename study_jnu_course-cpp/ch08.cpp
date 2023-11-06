#include <iostream>
using namespace std;
#define FN static void

namespace CH08_namespace {
	class Car {
	private:
		//int speed = 100; //��� C++���� ������ (����õ)
		int speed;
		int gear;

		const int cons;
		int& ref;
	public:
		//Initialization list: �ʱ�ȭ ����Ʈ
		Car() : cons(0), ref(speed) {
			cout << "������ Car() ȣ��" << endl;
			speed = 0;
			gear = 0;
			//cons = 10;	������ ���԰� ����; �Ұ���
			//ref = &speed;	���������� �Ұ���
		}
		Car(int p) : cons(0), ref(speed) {
			cout << "������ Car(int) ȣ��" << endl;
			speed = p;
			gear = p;
		}
		//�ı���
		~Car() {
			cout << "�ı��� ~Car() ȣ��" << endl;
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
		//Car myCar = { 10, 0 }; #private ������ �Ұ���
		// Car()			�⺻ ������
		// Car(const Car &)	���� ������
		// Car(Car &&)		�̵� ������
		// Car(int k)		��ȯ ������

		Car myCar = Car();
		myCar.accel();
		myCar.accel();
		cout << myCar.accel() << endl;

		Car myCar2(100);
		cout << myCar.accel() << endl;
	}
};