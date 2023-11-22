#include <iostream>
#include <vector>
#include <map>
using namespace std;

namespace cpp18_1 {
//1. Hash table 

class phonebook
{
    map<int, vector<string>> table;

public:
    int hash(const string& s)
    {
        int ret = 0;
        for (auto i : s) ret += i;
        return ret % 8;
    }

    //my function
    bool find(int key) {
        return table.find(key) != table.end();
    }

    void input(const string& name, const string& number)
    {
        // write code here
        int key = hash(name);

        if (!find(key))
            table[key] = vector<string>({ number });
        else 
            table[key].push_back(number);        
    }

    void get(const string& name)
    {
        // write code here
        int key = hash(name);
        if (!find(key)) {
            cout << "*** not found \"" << name << "\" / key: " << -1 << "\n\n\n";
            return;
        }

        cout << "*** get: " << name << " / key: " << key << endl;
        vector<string>& nums = table[key];
        for (int i = 0; i < nums.size(); i++) {
            cout << nums[i] << endl;
        }
        cout << endl << endl;
    }
};

int main() {
    phonebook p;

    p.input("chonnam", "111-1111");
    p.input("national", "222-2222");
    p.input("university", "333-3333");
    p.input("c++", "444-4444");
    p.input("programming", "555-5555");
    p.input("and", "666-6666");
    p.input("practice", "777-7777");

    p.get("chonnam");
    p.get("national");
    p.get("university");
    p.get("c++");
    p.get("this is no-input string"); //not found
    p.get("programming");
    p.get("and");
    p.get("practice");

    return 0;
}

}



int main() { cpp18_1::main(); }