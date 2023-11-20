#include <iostream>
#include <vector>
#include <list>
using namespace std;

namespace CH11 {
#define FN static void


	//my strchr (xstrchr)
	//수정을 거듭하다 보니... find가 되었다!
	template <typename T1, typename T2>
	T1* xfind(T1* first, T1* last, T2 c) {
		while (first != last && *first != c) 
			++first;
		//return first == last ? 0 : first;
		return first;
	}

	class RunExample {
	public:
		FN construct_string() {
			//string(const char*)
			string one("Lottery Winner!");
			cout << one << endl;

			//string(size_type, char)
			string two(20, '$');
			cout << two << endl;

			//string(const string&)
			string three(one);
			cout << three << endl;
			one += " Oops!";
			cout << one << endl;
			two = "Sorry! That was ";
			three[0] = 'P';

			//string()
			string four;
			four = two + three; //overloaded +, =
			cout << four << endl;

			char alls[] = "All's well that ends well";
			//string(const char*, size_type);
			string five(alls, 20);
			cout << five << "!\n";

			//string(Iter begin, Iter end);
			string six(alls + 6, alls + 10);
			cout << six << ", ";

			//string(Iter begin, Iter end);
			string seven(&five[6], &five[10]);
			cout << seven << "...\n";

			//string (const string&, size_type pos, size_type n = npos)
			string eight(four, 7, 16);

			cout << eight << " in motion!" << endl;


		}
		FN string_functions() {
			string str("The quick brown fox jumps over the lazy dog");

			cout << str.length() << endl;
			cout << str.find("fox") << endl;
			cout << str.find("b") << endl;
			cout << str.find("bear") << endl;

			str.erase(str.find("quick"), 6);
			cout << str << endl;

			str.replace(str.find("fox"), 3, "bear");
			cout << str << endl;

			str.append(", wow!");
			cout << str << endl;

			string apple = "apple";
			string big = "big";
			string cat = "cat";

			cout << (apple > big) << endl;
			cout << (apple < big) << endl;
			cout << (big == cat) << endl;


		}
		FN smart_ptr() {
			//스마트포인터도 STL !!
			shared_ptr<int> p1(new int);
			unique_ptr<double> p2(new double);

			//but static_cast<>, const_cast<> 이런건 연산자! STL X
			*p1 == 10;
			*p2 == 20;

			cout << *p1 + *p2 << endl;


		}
		FN vector_ex() {
			vector<int> vec1;
			vec1.push_back(10);
			vec1.push_back(20);
			vec1.push_back(30);

			cout << vec1.size() << endl;
			cout << vec1.capacity() << endl << endl;

			vector<int> vec2(20);
			cout << vec2.size() << endl;
			cout << vec2.capacity() << endl << endl;

			for (int i = 0; i < vec1.size(); i++) {
				cout << vec1[i] << endl;
			}
			cout << endl;

			while (!vec1.empty()) {
				cout << vec1.back() << endl;
				vec1.pop_back();
			}


		}
		FN vector_iterator_ex() {
			vector<int> vec1;
			//list<int> vec1; //컨테이너만 바껴도 OK
			vec1.push_back(10);
			vec1.push_back(20);
			vec1.push_back(30);

			//iterator
			vector<int>::iterator it;
			//list<int>::iterator it;
			for (it = begin(vec1); it != end(vec1); it++)
				cout << *it << endl;
			cout << endl;

			//auto
			for (auto it2 = vec1.begin(); it2 != vec1.end(); it2++)
				cout << *it2 << endl;
			cout << endl;

			// ranged-loop
			for (auto it3 : vec1)
				cout << it3 << endl;
			cout << endl;


		}
		FN generic_algorithm_ex1() {
			char s[] = "abcdefg";
			char* p = nullptr;

			//string s에서 주어진 문자 'c'를 찾는다.
			//찾으면 문자 위치 포인터가 반환되고, 없으면 NULL 리턴
			p = strchr(s, 'c');

			//my strchr
			p = xfind(s, s+5, 'f');
			if (p != nullptr) cout << *p << endl;			
			else cout << "it was null;\n";

			//double이어도 가능
			double d[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
			double* q = xfind(d, d + 5, 10);
			if (q != nullptr) cout << *q << endl;
			else cout << "it was null;\n";

		}
		FN my_xfind_test() {
			vector<double> arr = { 1.0, 2.0, 3.0, 4.0, 5.0 };
			double* q;
			//q = xfind(arr, arr + 5, 10); 이게 가능하려면 어떻게..? (iterator 구현!)
			cout << *q << endl;
		}


	};


}

//int main() { CH11::RunExample::generic_algorithm_ex1(); }