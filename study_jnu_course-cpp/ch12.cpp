#include <iostream>
using namespace std;

namespace CH12 {
#define FN static void
	class Animal {
		string howl;
	public:
		Animal() { howl = ""; }
		void cry() { cout << howl << endl; };
		void eat() { cout << "Feed feed!" << endl; }
		void run() { cout << "Run Run!" << endl; }

		Animal(const string& sound) : howl(sound) {}
	};

	class Cat : public Animal {
	public:
		Cat(const string& sound) : Animal(sound) {}
		void nyang() { cout << "Cat cat cat" << endl; }
	};

	class Cow : public Animal {
		int val;
	public:
		Cow(const string& sound) : val(5), Animal(sound) {}
		void Umme() { cout << "Cow Cow Cow" << endl; }
	};

	class Dog : public Animal {
		Dog() : Animal() {}
	public:
		Dog(const string& sound) : Animal(sound) { cout << "Dog" << endl; }
		Dog(int age) : Dog("") { cout << "Dog aged " << age << endl; }
		void bark() { cout << "Mung!" << endl; }
	};

	class MyDog : public Dog {
	public:
		//MyDog() {} compiler error
		MyDog(const string& s) : Dog(s) {}
		MyDog(int age) : Dog(age) {}
		void bark() { cout << "Wal!" << endl; }
		void bark(bool feed) { cout << "Krre re ek mmd kkaksdf" << endl; }
	};
	
	class RunExample {
	public:
		FN func() {
			Dog d("mung mung");
			Cat c("nyang nyang");
			Cow w("Ummmmmmmmmmmmmmeeeeeeeeeeeeee");
			d.cry();
			c.cry();
			w.cry();


		}
		FN func2() {
			Dog dog("");
			MyDog myDog("");
			MyDog mDog(5);

			dog.bark();
			myDog.bark();
			myDog.bark(true);
		}




	};


}

int main() { CH12::RunExample::func2(); }