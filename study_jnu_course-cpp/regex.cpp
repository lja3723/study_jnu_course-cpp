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
            
        //Additional Settings
            //.less_details({ 1, 2 }) //set less details
            //.disable({ 13 })      //desable some testcases
            //.disable_all()
            //.enable({13})
            .run(); //run unit test

        cout << "���� ������ �ð�: 21��(ȭ) ���� 6:07" << endl;

        cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
        string test_str = "wersAABBBtfdAdifuBBtitaAtBBABTts";
        cout << test_str << endl;
        for (int i = 0; i <= test_str.size() + 1; i++) {
            auto result = MySimpleRegex::match("A*B*", test_str, i);
            cout << result << endl;
        }
        return 0;
    }
}

int main() { assignment1::main(); }
