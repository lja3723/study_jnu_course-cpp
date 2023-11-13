//깃 컨벤션 참고 https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md
#include "MyUnitTest.h"

namespace assignment1 {
    using namespace std;

    int main() {
        MyUnitTest()
            .less_details_all()
            //.set_summary_less_details()
            .disable({1, 8, 9})
            .run();        

        return 0;
    }

    int readerTest() {
        MyUnitTest t;
        t.reader_test("test_data.txt");

        return 0;
    }

    void stoitest() {
        string s = "ad23";
        int k = stoi(s);
        cout << k << endl;
    }
}

//int main() { assignment1::readerTest(); }
