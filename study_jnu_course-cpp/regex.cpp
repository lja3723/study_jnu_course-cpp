//�� ������ ���� https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md
#include "MyUnitTest.h"

namespace assignment1 {
    using namespace std;

    int main() {
        string test_data = "test_data.txt";
        MyUnitTest unit_test(test_data);
        if (!unit_test.is_open()) {
            cout << "\"" << test_data << "\" ������ �������� �ʽ��ϴ�. ";
            cout << "�׽�Ʈ�� ������ �� �����ϴ�." << endl;
            return -1;
        }
        
        unit_test
        //���ϴ� �ɼǿ� �ּ� ���� �� ���
        //Print Settings (default setting)
            //.set_newline(80)
            //.set_underline_marker('^')
            //.set_indent_level(3)

        //Basic Settings (default setting)
            //.set_summary_more_details()
            //.enable_all()
            //.more_details_all()
            .less_details_all()
            //.more_details({21, 22, 23})
            
        //Additional Settings
            //.less_details({ 1, 2 }) //set less details
            //.disable({ 20 })      //desable some testcases
            //.disable_all()
            //.enable({52})
            .run(); //run unit test

        cout << "���� ������ �ð�: " << __TIMESTAMP__ << endl;


        return 0;
    }

    void test() {
        cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
        string test_str = "wersAABBBtfdAdifuBBtitaAtBBABTts";
        cout << test_str << endl;
        for (int i = 0; i <= test_str.size() + 1; i++) {
            auto result = MySimpleRegex::match("A*B*", test_str, i);
            cout << result << endl;
        }
    }

    void test1() {
        vector<int> v = { 3, 6, 4, 3, 0, 7, 0, 1, 2 };

        cout << "size: " << v.size() << endl;
        for (auto& k : v) cout << k << " ";
        cout << endl;

        v.erase(remove_if(v.begin(), v.end(), [](int& e) { return e == 110; }), v.end());

        cout << "size: " << v.size() << endl;
        for (auto& k : v) cout << k << " ";
        cout << endl;
    }

}

int main() { assignment1::main(); }