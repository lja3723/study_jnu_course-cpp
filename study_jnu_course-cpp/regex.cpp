//깃 컨벤션 참고 https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md
#include "MyUnitTest.h"

namespace assignment1 {
    using namespace std;

    int main() {
        string test_data = "test_data.txt";
        MyUnitTest test(test_data);
        if (!test.is_open()) {
            cout << "\"" << test_data << "\" 파일이 존재하지 않습니다. 테스트를 수행할 수 없습니다." << endl;
            return -1;
        }

        test
            .less_details_all()
            //.set_summary_less_details()
            .disable({})
            .run();        

        return 0;
    }
}

int main() { assignment1::main(); }
