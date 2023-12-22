﻿#include "MyUnitTest.h"

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
            
        //Additional Settings
            //.less_details({ 1, 2 })   //set less details
            //.disable({ 20 })          //desable some testcases
            //.disable_all()
            //.enable({66, 67})
            //.less_details_all()
            .run(); //run unit test

        return 0;
    }

}

int main() { assignment1::main(); }