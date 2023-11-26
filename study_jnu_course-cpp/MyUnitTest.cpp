#include "MyUnitTest.h"

namespace assignment1 {
	

/*************************************/
/*      테스트 실행 및 관련 유틸 함수   */
/*      실행 순서대로 배치되었음        */
/*************************************/
MyUnitTest::MyUnitTest(const char* test_filename) :
    m_newline(80),
    m_underline_marker('^'),
    m_no_syntax_error(true)
{
    set_indent_level(3);
    clear_tests();
    file_open(test_filename);

    set_summary_more_details();
    enable_all().more_details_all(); //default settings


}
void MyUnitTest::clear_tests() {
    tests.clear();
    //dummy test (인덱스를 1부터 사용할 것임)
    tests.push_back({ 0, "", 0, vector<string>(), vector<ranges>() });


}
void MyUnitTest::file_open(const char* name) {
    TestDataFileReader reader;
    m_is_file_open = reader.open(name);
    if (!m_is_file_open) return;

    m_no_syntax_error = reader.read(tests);
    if (m_no_syntax_error)
        cout << "\"" << name << "\" 파일을 성공적으로 읽었습니다." << endl;


}
bool MyUnitTest::is_open() { 
    return m_is_file_open; 


}
void MyUnitTest::run() {
    if (m_is_file_open && m_no_syntax_error) {
        run_tests();
    }


}
void MyUnitTest::run_tests() {
    print_run_title();

    //비활성화된 테스트 리스트 구하기
    set<int> disabled;
    for (int t = 1; t < tests.size(); t++)
        if (!tests[t].enabled) disabled.insert(tests[t].number);

    //테스트 통계 출력 위한 결과 수집변수
    vector<int> successed_list;
    vector<int> failed_list;

    //각 테스트 게이스에 대해 테스트 수행
    for (int i = 1; i < tests.size(); i++) {
        Test& test = tests[i];

        //disabled된 테스트 스킵
        if (disabled.find(test.number) != disabled.end()) continue;

        //테스트 수행
        //results.size() == 1인 경우 regex 문법이 유효하지 않다는 의미
            //results[0] == true : 유효하지 않는 테스트 통과
            //results[1] == false : 유효해야 하지만 유효하지 않음
        vector<bool> results = run_test(test);

        //테스트 성공여부 계산
        bool test_successed = true;
        for (bool result : results) test_successed &= result;

        if (test_successed) {
            successed_list.push_back(test.number);
            print_successed_test(test);
        }
        else {
            failed_list.push_back(test.number);
            print_failed_test(test, results);
        }
    }

    //테스트 종합 결과 출력
    print_tests_summary(disabled, successed_list, failed_list);


}
vector<bool> MyUnitTest::run_test(const Test& t) {
    vector<bool> ret;

    //정규식을 컴파일한다.
    MySimpleRegex::compiled cp = MySimpleRegex::compile(t.regex);

    //정규식 문법 유효성 테스트
    ret.push_back(t.syntax_valid == cp.is_valid());

    //각 케이스 문자열마다 기대값(expect)과 실행값(result)이 같은지 비교한다.
    for (int i = 0; cp.is_valid() && i < t.test.size(); i++) {
        ranges result = cp.match_all(t.test[i]);
        ret.push_back(assertEqual(t.expect[i], result));
    }

    return ret;


}
bool MyUnitTest::assertEqual(const ranges& expect, const ranges& result) {
    if (expect.size() != result.size()) return false;

    for (int i = 0; i < expect.size(); i++) {
        if (expect[i].start != result[i].start) return false;
        if (expect[i].end != result[i].end)     return false;
        if (expect[i].is_valid != result[i].is_valid) return false;
    }
    return true;


}



/*******************   테스트 옵션   *******************/
MyUnitTest& MyUnitTest::set_newline(int newline) {
    m_newline = newline;
    return *this;
}
MyUnitTest& MyUnitTest::set_underline_marker(char marker) {
    m_underline_marker = marker;
    return *this;
}
MyUnitTest& MyUnitTest::set_indent_level(int indent_level) {
    m_indent = "";
    for (int i = 0; i < indent_level; i++) m_indent += " ";
    return *this;
}
MyUnitTest& MyUnitTest::set_summary_more_details() {
    details_summary_mode = true;
    return *this;
}
MyUnitTest& MyUnitTest::set_summary_less_details() {
    details_summary_mode = false;
    return *this;
}

MyUnitTest& MyUnitTest::more_details(int test) { return more_details({ test }); }
MyUnitTest& MyUnitTest::more_details(initializer_list<int> tests_list) {
    for (int test_num : tests_list) tests[test_num].details = true;
    return *this;
}
MyUnitTest& MyUnitTest::less_details(int test) { return less_details({ test }); }
MyUnitTest& MyUnitTest::less_details(initializer_list<int> tests_list) {
    for (int test_num : tests_list) tests[test_num].details = false;
    return *this;


}

MyUnitTest& MyUnitTest::enable(int test) { return enable({ test }); }
MyUnitTest& MyUnitTest::enable(initializer_list<int> tests_list) {
    cout << "initializer_list" << endl;
    for (int test_num : tests_list) tests[test_num].enabled = true;
    return *this;
}
MyUnitTest& MyUnitTest::disable(int test) { return disable({ test }); }
MyUnitTest& MyUnitTest::disable(initializer_list<int> tests_list) {
    for (int test_num : tests_list) tests[test_num].enabled = false;
    return *this;


}

MyUnitTest& MyUnitTest::more_details_all() {
    for (Test& t : tests) t.details = true;
    return *this;
}
MyUnitTest& MyUnitTest::less_details_all() {
    for (Test& t : tests) t.details = false;
    return *this;
}
MyUnitTest& MyUnitTest::enable_all() {
    for (Test& t : tests) t.enabled = true;
    return *this;
}
MyUnitTest& MyUnitTest::disable_all() {
    for (Test& t : tests) t.enabled = false;
    return *this;
}



/*******************   출력 함수   *******************/
void MyUnitTest::print_run_title() {
    string t_indent = m_indent + m_indent;
    cout << "\n\n";
    cout << t_indent << "****************************************************************" << endl;
    cout << t_indent << "****                                                        ****" << endl;
    cout << t_indent << "****                 MySimpleRegex Unit Test                ****" << endl;
    cout << t_indent << "****                                                        ****" << endl;
    cout << t_indent << "****************************************************************" << endl;
    cout << "\n\n";


}
void MyUnitTest::print_test_title(string label_left, const Test& t, string label_right) {
    cout << "\n\n\n";
    cout << label_left << t.number << " \"" << t.regex << "\"" << label_right << endl;


}
void MyUnitTest::print_match_range(string title, const ranges& match_result, const Test& t, int elem) {
    cout << m_indent << title << endl;
    if (match_result.empty()) {
        cout << m_indent << t.test[elem] << " -> 일치구간 없음" << "\n\n";
        return;
    }

    print_match_underline(match_result);
    for (const ranged_string& mt : match_result) {
        cout << m_indent << m_indent;
        cout << "-> " << mt << endl;
    }
    cout << endl;


}
void MyUnitTest::print_match_underline(const ranges& result) {
    //매치된 영역에 밑줄 표시; 입력 문자열이 길면 newline마다 개행됨
    if (result.empty()) return;

    const string& ref = result[0].ref;

    //문자열과 밑줄을 끊어서 출력
    for (size_t i = 0, cur_match = 0; i < ref.size(); i += m_newline) {
        size_t len = i + m_newline <= ref.size() ? m_newline : ref.size() - i;
        cout << m_indent << ref.substr(i, len) << endl; //참조 문자열 출력  
        if (cur_match >= result.size() || i + len <= result[cur_match].start) {
            cout << endl;
            continue;
        }

        cout << m_indent;
        for (size_t j = i; j < i + len; j++) {
            const ranged_string& str = result[cur_match];
            cout << (str.start <= j && j < str.end ? m_underline_marker : ' '); // 밑줄 출력
            if (j + 1 >= str.end) cur_match++;
            if (cur_match >= result.size()) break;
        }
        cout << endl;
    }


}
void MyUnitTest::print_successed_test(const Test& t) {
    if (!t.details) {
        cout << "테스트 #" << t.number << " 통과" << endl;
        return;
    }

    print_test_title("테스트 #", t, " 통과 (more detail mode enabled)");
    MySimpleRegex::compiled cp = MySimpleRegex::compile(t.regex);

    if (cp.is_valid()) {
        for (int i = 0; i < t.test.size(); i++) {
            ranges result = cp.match_all(t.test[i]);
            cout << "case #" << (i + 1) << endl;
            print_match_range("< 실행결과 >", result, t, i);
        }
    }
    else {
        cout << m_indent << "< 실행결과 >" << endl;
        cout << m_indent << m_indent << "-> 정규표현식 문법이 유효하지 않음(기대결과와 일치)" << endl;
    }
    cout << "\n\n";


}
void MyUnitTest::print_failed_test(const Test& t, vector<bool>& results) {
    //실패한 테스트 제목
    print_test_title("<!>>>>>>> 테스트 #", t, string(" 실패") 
        + (t.details ? " (more detail mode enabled)" : ""));

    //실패한 테스트를 재현하기 위해 정규식 재컴파일
    MySimpleRegex::compiled cp = MySimpleRegex::compile(t.regex);

    //정규식 문법이 올바른 경우 - 테스트케이스가 틀림
    if (results[0] == true) {
        for (int i = 0; i < results.size() - 1; i++) {
            if (!t.details && results[i + 1] == true) continue;
            ranges result = cp.match_all(t.test[i]);

            //실패한 케이스 출력
            cout << "case #" << (i + 1) << (results[i + 1] ? " (통과)" : " (실패)") << endl;
            if (!results[i + 1])
                print_match_range("< 기대결과 >", t.expect[i], t, i);
            print_match_range("< 실행결과 >", result, t, i);
        }
    }

    //정규표현식 문법이 틀렸지만 맞았다고 나온 경우
    else if (cp.is_valid()) {
        cout << m_indent << "< 기대결과 >" << endl;
        cout << m_indent << m_indent << "-> 정규표현식 문법이 유효하지 않음" << endl;
        cout << m_indent << "< 실행결과 >" << endl;
        cout << m_indent << m_indent << "-> 정규표현식 문법이 유효함" << endl;
    }

    //정규표현식 문법이 맞지만 틀렸다고 나온 경우
    else {
        cout << m_indent << "< 기대결과 >" << endl;
        cout << m_indent << m_indent << "-> 정규표현식 문법이 유효함" << endl;
        cout << m_indent << "< 실행결과 >" << endl;
        cout << m_indent << m_indent << "-> 정규표현식 문법이 유효하지 않음" << endl << endl;

        cout << m_indent << "아래와 같은 실행결과가 기대되었음" << endl << endl;
        for (int i = 0; i < t.test.size(); i++) {
            cout << "case #" << (i + 1) << endl;
            print_match_range("< 기대결과 >", t.expect[i], t, i);
        }
    }
    cout << "\n\n";


}
void MyUnitTest::print_tests_summary(set<int>& disabled, vector<int>& successed_list, vector<int>& failed_list) {
    cout << "\n\n";
    if (details_summary_mode) {
        string lines = "=========================";
        size_t tests_size = tests.size() - 1;
        cout << lines << "  [테스트 종합 결과]  " << lines << endl;
        cout << "총 테스트케이스 수:\t" << tests_size << " 개\n";
        if (!disabled.empty())
            cout << "비활성화된 테스트 수:\t" << disabled.size() << " 개\n";
        cout << "수행한 테스트 수:\t" << (tests_size - disabled.size()) << " 개\n";
        cout << "성공한 테스트케이스 수:\t" << (successed_list.size()) << " 개\n";
        if (!failed_list.empty())
            cout << "실패한 테스트케이스 수:\t" << failed_list.size() << " 개\n";
        cout << endl;

        if (!disabled.empty()) {
            cout << "비활성화된 테스트케이스 목록:" << endl << m_indent << m_indent;
            for (int test : disabled) cout << test << ";  ";
            cout << endl;
        }
        if (!disabled.empty()) {
            cout << "성공한 테스트케이스 목록:" << endl << m_indent << m_indent;
            for (int test : successed_list) cout << test << ";  ";
            cout << endl;
        }
        if (!failed_list.empty()) {
            cout << "실패한 테스트케이스 목록:" << endl << m_indent << m_indent;
            for (int test : failed_list) cout << test << ";  ";
            cout << endl;
        }
    }
    else {
        if (failed_list.empty()) {
            cout << "테스트를 모두 통과하였습니다." << endl;
        }
        else {
            cout << "실패한 테스트가 있습니다." << endl << m_indent << "리스트:  ";
            for (int test : failed_list) cout << test << ";  ";
            cout << endl;
        }
        if (disabled.empty()) return;
        cout << m_indent << "비활성화된 테스트 리스트:  ";
        for (int test : disabled) cout << test << ";  ";
        cout << endl;
    }

    //less_details_all이 활성화인지 확인
    bool less_details_all = true;
    for (int i = 1; i < tests.size(); i++)
        less_details_all &= !tests[i].details;
    if (less_details_all) {
        cout << endl;
        cout << "간략화된 테스트 결과 보기가 켜져 있습니다." << endl;
        cout << "더 자세한 테스트 결과를 보시려면 run() 실행 이전에 more_details_all()을 실행하세요." << endl;
    }
    cout << "\n\n";
}



} //end of namespace assignment1