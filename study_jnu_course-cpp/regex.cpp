//깃 컨벤션 참고 https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md
#include "MyUnitTest.h"

namespace assignment1 {
    using namespace std;

    int main() {
        string test_data = "test_data.txt";
        MyUnitTest unit_test(test_data);
        if (!unit_test.is_open()) {
            cout << "\"" << test_data << "\" 파일이 존재하지 않습니다. ";
            cout << "테스트를 수행할 수 없습니다." << endl;
            return -1;
        }
        
        unit_test
        //원하는 옵션에 주석 해제 후 사용
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

        cout << "최종 컴파일 시간: " << __TIMESTAMP__ << endl;


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