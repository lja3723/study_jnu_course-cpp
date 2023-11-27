#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

namespace cpp19 {
	//1. Remove duplicate elements in the vector

	//my code

	int main() {
		vector<char> v = { 'a', 'b', 'b', 'a', 'e', 'd', 'd', 'b' };
		for (auto& i : v) cout << i << " "; cout << endl;

		sort(v.begin(), v.end());
		v.erase(unique(v.begin(), v.end()), v.end());
		
		for (auto& i : v) cout << i << " "; cout << endl;
		return 0;
	}
}
//int main() { cpp19::main(); }