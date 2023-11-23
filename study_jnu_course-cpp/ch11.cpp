#include <iostream>
#include <vector>
#include <list>

#include <algorithm> //sort (quick sort)
#include <functional> //less<>, greater<> 
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

	//new xfind
	//스마트포인터 지원
	template <typename T1, typename T2>
	T1 xfind_new(T1 first, T1 last, T2 c) {
		while (first != last && first != c)
			++first;
		return first;
	}


	template<class T> struct Node {
		T data;
		Node* next;
		Node(T _data, Node* _next) : data(_data), next(_next) {}
	};

	//slist를 xfind에 쓰기 위한 프록시(타입 변환 위한 객체)
	//첫 이름은 slist_proxy로 시작했지만, 이름을 slist_iterator로 바꿉시다.
	//이제 외부에 선언된 이 클래스를 내부로 선언합시다. => 아니, 내부선언 아님
	//이름이 iterator로 바뀜 => 노노 외부선언이니까 다시 slist_iterator
	template <typename T>
	class slist_iterator {
	private: Node<T>* current;
	public:
		slist_iterator(Node<T>* p = 0) : current(p) {}

		bool operator!=(const slist_iterator& p) { return current != p.current; }
		//bool operator!=(const T& d) { return current->data != d; }
		template<typename U> bool operator!=(U data) { return current->data != data; }

		T& operator*() { return current->data; }

		slist_iterator& operator++() {
			current = current->next;
			return *this;
		}

		slist_iterator& operator++(int) {
			slist_iterator ret = *this;
			current = current->next;
			return ret;
		}
	};

	template<typename T> class slist {
		Node<T>* head;
		/* OLD */ Node<T>* current;

	public:
		using iterator = slist_iterator<T>;
		//typedef slist_iterator<T> iterator;

		slist() : head(0), /*OLD*/current(0) {}
		void push_front(const T& a) { head = new Node<T>(a, head); }

		/* OLD */ T& operator*() { return current->data; }
		/* OLD */ slist<T>& begin_old() { current = head; return *this; }
		/* OLD */ slist& next_old() { current = current->next; return *this; }
		/* OLD */ bool end_old() { return current == 0; }

		iterator begin() { return iterator(head); }
		iterator end() { return iterator(0); }
	};

	struct Plus { int operator()(int a, int b) { return a + b; } };

	class PiggyBox {
		int total;
	public:
		PiggyBox(int init = 0) : total(init) {}
		int operator()(int money) { total += money; return total; }
	};

	void Sort(int* x, int n) { //bubble sort
		for (int i = 0; i < n; i++)
			//for (int j = 0; j < n; j++) //이건 내림차순 정렬됨 why?
			for (int j = i; j < n; j++)
				if (x[i] > x[j]) {
					int tmp = x[i];
					x[i] = x[j];
					x[j] = tmp;
				}
	}


	struct PRED { virtual int operator()(int, int) const = 0; };
	//이렇게 구현할 수도 있고 아니면 템플릿을 사용해도 됐었다
	void Sort_pred(int* x, int n, const PRED& cmp) { //bubble sort
		for (int i = 0; i < n; i++)
			//for (int j = 0; j < n; j++) //이건 내림차순 정렬됨 why?
			for (int j = i; j < n; j++)
				if (cmp(x[i], x[j])) {
					int tmp = x[i];
					x[i] = x[j];
					x[j] = tmp;
				}
	}
	struct UP	: public PRED { int operator()(int a, int b) const { return a > b; } };
	struct DOWN : public PRED { int operator()(int a, int b) const { return a < b; } };
	struct ABS	: public PRED { int operator()(int a, int b) const { return abs(a) > abs(b); } };

	template<typename PRED>
	void Sort_pred_template(int* x, int n, const PRED cmp) { //bubble sort
		for (int i = 0; i < n; i++)
			//for (int j = 0; j < n; j++) //이건 내림차순 정렬됨 why?
			for (int j = i; j < n; j++)
				if (cmp(x[i], x[j])) {
					int tmp = x[i];
					x[i] = x[j];
					x[j] = tmp;
				}
	}

	//소팅 함수의 최종버전
	template<typename T, typename PRED>
	void Sort_generic(T begin, T end, PRED cmp) { //bubble sort
		for (auto i = begin; i != end; i++)
			for (auto j = i; j != end; j++)
				if (cmp(*i, *j)) {
					auto tmp = *i;
					*i = *j;
					*j = tmp;
				}
	}

	template<typename T> class VECTOR_iterator {

	};

	template <class T>
	class VECTOR {
		T* mPtr;
		int mSize;
		int mCapacity;
		int mIndex;

	public:
		VECTOR(int s = 0) : mSize(s), mCapacity(s), mIndex(0) {
			mPtr = new T[mCapacity];
		}

		int size() const { return mSize; }
		int capacity() const { return mCapacity; }

		void resize(int s) {
			if (s <= mCapacity) mSize = s;
			else {
				mCapacity = s * 2; //Margin
				mSize = s;

				T* tmp = new T[mCapacity];
				memcpy(tmp, mPtr, mCapacity);
				delete[] mPtr;
				mPtr = tmp;
			}
		}

		void push_back(const T& val) {
			if (mIndex == mSize)
				resize(mSize + 1);

			*(mPtr + mIndex++) = val;
			mPtr[mIndex++] = val;
		}
		T& operator[](int idx) { return mPtr[idx]; }

		typedef VECTOR_iterator<T> iterator;
		iterator begin() { return iterator(mPtr); }
		iterator end() { return iterator(mPtr + mSize); }

		void _sort() {
			Sort_generic(begin(), end(), [](T a, T b) {return a > b; });
		}


	};


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
			//cout << *q << endl;


		}
		FN iterator_ex1() {
			slist<int> s;
			s.push_front(10); s.push_front(20);
			s.push_front(30); s.push_front(40);

			//OLD METHOD
			//아직은... 우리의 xfind와 호환되지는 않음
			//proxy를 만들어봅시다
			for (s.begin_old(); !s.end_old(); s.next_old()) {
				cout << *s << endl;
			}
			cout << endl;

			//NEW METHOD using proxy
			//slist_proxy의 새 이름: slist_iterator
			//slist_iterator를 내부 클래스로 옮김 -> 이름은 slist<T>::iterator
			slist<int>::iterator p;
			for (p = s.begin(); p != s.end(); p++) {
				cout << *p << endl;
			}
			cout << endl;

			//xfind와 호환될까??!?
			slist<int>::iterator it = xfind_new(s.begin(), s.end(), 30);

			cout << *it << endl;
			cout << endl;
			//된다. iterator는 이렇게 구현되는 것이었습니다~


		}
		FN function_object() {
			Plus p;
			cout << p(1, 2) << endl;
		}
		FN func_object_state_ex() {
			PiggyBox pig;
			cout << "money(100): " << pig(100) << endl;
			cout << "money(500): " << pig(500) << endl;
			cout << "money(2000): " << pig(2000) << endl;


		}
		FN usage_of_funcobj() {
			int arr[] = { 6,5,9,4,8,1,3 };
			Sort(arr, 7);
			for (auto i : arr) cout << i << " ";
			cout << endl << endl;


			int arr2[] = { -6, -5, 9, 4, -8, 1, -3 };

			Sort_pred(arr2, 7, UP());
			for (auto i : arr2) cout << i << " ";
			cout << endl;

			Sort_pred_template(arr2, 7, DOWN());
			for (auto i : arr2) cout << i << " ";
			cout << endl;

			Sort_pred_template(arr2, 7, [](int a, int b) {return abs(a) > abs(b); });
			for (auto i : arr2) cout << i << " ";
			cout << endl;



			vector<int> vec = { -6, -5, 9, 4, -8, 1, -3 };
			Sort_generic(vec.begin(), vec.end(), [](int a, int b) { return a > b; });
			for (auto i : vec) cout << i << " ";


		}
		FN STL_sort_ex() {
			int x[10] = { 1, 3, 5, 7, 9, 2, 4, 6, 8, 10 };
			less<int> cmp1;
			greater<int> cmp2;

			sort(x, x + 10, cmp1); //sort(int*, int*, less);
			for (auto i : x) cout << i << " ";
			cout << endl;

			sort(x, x + 10, cmp2);
			for (auto i : x) cout << i << " ";
			cout << endl;


		}
		FN my_vector_ex() {
			VECTOR<int> v;

			v.push_back(10);
			v.push_back(20);

			cout << v.size() << endl;
			cout << v.capacity() << endl;

			v[0] = 30;

			cout << v[0] << endl;
			cout << v[1] << endl;
		}



	};


}

//int main() { CH11::RunExample::my_vector_ex(); }