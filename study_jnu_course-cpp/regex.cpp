//�� ������ ���� https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md
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

    int main1() {
        TestDataReader::read("test_data.txt");
        return 0;
    }
}

int main() { assignment1::main(); }