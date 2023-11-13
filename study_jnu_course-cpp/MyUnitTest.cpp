#include "MyUnitTest.h"

namespace assignment1 {
	

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
    
void MyUnitTest::print_test_title(string label_left, int test_number, string label_right) {
    cout << "\n\n\n";
    cout << label_left << test_number << " \"" << tests[test_number].regex << "\"" << label_right << endl;
}

void MyUnitTest::print_match_range(string title, vector<ranged_string>& match_result, int test_case, int elem) {
    cout << m_indent << title << endl;
    if (match_result.empty()) {
        cout << m_indent << tests[test_case].test[elem] << " -> 일치구간 없음" << "\n\n";
        return;
    }

    print_match_underline(match_result);
    for (const ranged_string& mt : match_result) {
        cout << m_indent << m_indent;
        cout << "-> range [" << mt.start << "," << mt.end << ") " << "\"" << mt.group() << "\"\n";
    }
    cout << endl;
}

//매치된 영역에 밑줄 표시; 입력 문자열이 길면 newline마다 개행됨
void MyUnitTest::print_match_underline(const vector<ranged_string>& result) {
    if (result.empty()) return;

    //TODO: ref가 NULL인 버그 발생
    //해결하자
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

void MyUnitTest::print_successed_test(vector<int>& successed_list, int test_number) {
    successed_list.push_back(test_number);
    if (!tests[test_number].details) {
        cout << "테스트 #" << test_number << " 통과" << endl;
        return;
    }

    print_test_title("테스트 #", test_number, " 통과 (more detail mode enabled)");
    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex, test_number);

    for (int i = 0; i < tests[test_number].test.size(); i++) {
        vector<ranged_string> result = cp.match_all(tests[test_number].test[i]);
        cout << "case #" << (i + 1) << endl;
        print_match_range("< 실행결과 >", result, test_number, i);
    }
    cout << "\n\n";
}

void MyUnitTest::print_failed_test(vector<int>& failed_list, vector<bool>& results, int test_number) {
    failed_list.push_back(test_number);
    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex, test_number);
    bool title_printed = false;
    for (int i = 0; i < results.size(); i++) {
        if (!tests[test_number].details && results[i] == true) continue;
        vector<ranged_string> result = cp.match_all(tests[test_number].test[i]);

        //실패한 테스트 제목 출력 (최초 1회만)
        if (!title_printed) {
            title_printed = true;
            string label_right = "실패";
            label_right += tests[i].details ? " (more detail mode enabled)" : "";
            print_test_title("<!>>>>>>> 테스트 #", test_number, label_right);
        }

        //실패한 케이스 출력
        cout << "case #" << (i + 1) << (results[i] ? " (통과)" : " (실패)") << endl;
        if (!results[i])
            print_match_range("< 기대결과 >", tests[test_number].expect[i], test_number, i);
        print_match_range("< 실행결과 >", result, test_number, i);
    }
    cout << "\n\n";
}

void MyUnitTest::print_tests_summary(set<int>& disabled, vector<int>& successed_list, vector<int>& failed_list) {
    cout << "\n\n";
    if (details_summary_mode) {
        string lines = "=========================";
        unsigned tests_size = unsigned(tests.size() - 1);
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



/*******************   테스트 실행 함수   *******************/
bool MyUnitTest::assertEqual(vector<ranged_string>& expect, vector<ranged_string>& result) {
    if (expect.size() != result.size()) return false;

    for (int i = 0; i < expect.size(); i++) {
        if (expect[i].start != result[i].start) return false;
        if (expect[i].end != result[i].end)     return false;
        if (expect[i].is_valid != result[i].is_valid) return false;
    }
    return true;
}

vector<bool> MyUnitTest::run_test(int t) {
    vector<bool> ret;

    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[t].regex, t);
    for (int i = 0; i < tests[t].test.size(); i++) {
        vector<ranged_string> list_all = cp.match_all(tests[t].test[i]);
        ret.push_back(assertEqual(tests[t].expect[i], list_all));
    }

    return ret;
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
    for (int t = 1; t < tests.size(); t++) {
        if (disabled.find(t) != disabled.end()) continue; //disabled된 테스트 스킵
        vector<bool> results = run_test(tests[t].number); //테스트 수행

        //테스트 성공여부
        bool test_successed = true;
        for (bool result : results) test_successed &= result;

        if (test_successed)
            print_successed_test(successed_list, t);
        else
            print_failed_test(failed_list, results, t);
    }

    //테스트 종합 결과 출력
    print_tests_summary(disabled, successed_list, failed_list);
}



/*******************   테스트 정의함수   *******************/
void MyUnitTest::test1() {
    int t = 1;
    static vector<string> test({
        "abc",
        "ade",
        "aaaaabbbbabcaaaaaadeaaaaabc",
        "abbbbbbbbdddddddfdfdfdfdf",
        "zzosiduabcfdsfdafdadere",
        "aosisudababaifudiaddadeidufodi",
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij"
        "asdhfaisdhoidofuhajekadadeaabcclaksdhfvnasjmaflsdmlfvaadcuhsvkdfhaksjdlakfjdfasdcaksdhflashdljhkakl;abclalwadeehmrua3kwinldawmeij" });

    tests.push_back({
        t,  "abc|ade", test,
    {{
        ranged_string(test[0], 0, 3, true)},
    {
        ranged_string(test[1], 0, 3, true) },
    {
        ranged_string(test[2], 9, 12, true),
        ranged_string(test[2], 17, 20, true),
        ranged_string(test[2], 24, 27, true) },
    { },
    {
        ranged_string(test[4], 7, 10, true),
        ranged_string(test[4], 18, 21, true), },
    {
        ranged_string(test[5], 20, 23, true) },
    {
        ranged_string(test[6], 23, 26, true),
        ranged_string(test[6], 27, 30, true),
        ranged_string(test[6], 100, 103, true),
        ranged_string(test[6], 107, 110, true)}} });
}
void MyUnitTest::test2() {
    int t = 2;
    static vector<string> test({
        "aba",
        "bab",
        "baba",
        "ababababababababababab" });

    tests.push_back({
        t, "aba", test,
    {{
        ranged_string(test[0], 0, 3, true)},
    { },
    {
        ranged_string(test[2], 1, 4, true)},
    {
        ranged_string(test[3], 0, 3, true),
        ranged_string(test[3], 4, 7, true),
        ranged_string(test[3], 8, 11, true),
        ranged_string(test[3], 12, 15, true),
        ranged_string(test[3], 16, 19, true)}} });
}
void MyUnitTest::test3() {
    int t = 3;
    static vector<string> test({
        "NKNQ",
        "NKOPQ",
        "NK",
        "OPQ",
        "NKabcabcabcNQ",
        "oosoiduuufufffii",
        "NKABCABCNQ",
        "NKabcABCabcABCABCabcABCNQ",
        "NKOPOPOPOPQ",
        "sofieNKNQofisNOPQkfuNKOPOPQlzNKABCNQksofuNKabcNQtttat" });

    tests.push_back({
        t, "NK((abc|ABC)*N|(OP)+)Q", test,
    {{
        ranged_string(test[0], 0, 4, true)},
    {
        ranged_string(test[1], 0, 5, true)},
    { },
    { },
    {
        ranged_string(test[4], 0, 13, true)},
    { },
    {
        ranged_string(test[6], 0, 10, true)},
    {
        ranged_string(test[7], 0, 25, true)},
    {
        ranged_string(test[8], 0, 11, true)},
    {
        ranged_string(test[9], 5, 9, true),
        ranged_string(test[9], 20, 27, true),
        ranged_string(test[9], 29, 36, true),
        ranged_string(test[9], 41, 48, true)}} });
}
void MyUnitTest::test4() {
    int t = 4;
    static vector<string> test({
        "a",
        "ac",
        "abc",
        "abbc",
        "abbbbbb",
        "abbbc",
        "abbbbbc",
        "abbbbababcccccabbbbcabbbbabacacabccccab" });

    tests.push_back({
        t, "ab+c", test,
    {{ },
    { },
    {
        ranged_string(test[2], 0, 3, true)},
    {
        ranged_string(test[3], 0, 4, true)},
    { },
    {
        ranged_string(test[5], 0, 5, true)},
    {
        ranged_string(test[6], 0, 7, true)},
    {
        ranged_string(test[7], 7, 10, true),
        ranged_string(test[7], 14, 20, true),
        ranged_string(test[7], 31, 34, true)}} });
}
void MyUnitTest::test5() {
    int t = 5;
    static vector<string> test({
        "a",
        "aaa",
        "aaaabb",
        "aabbcc",
        "ab",
        "abc",
        "abcc",
        "abccc",
        "abcccccc",
        "kkkabcccccccciabccccccabccccckkkcccccabccccccccccco" });

    tests.push_back({
        t, "abc+", test,
    {{ }, { }, { }, { }, { },
    {
        ranged_string(test[5], 0, 3, true)},
    {
        ranged_string(test[6], 0, 4, true)},
    {
        ranged_string(test[7], 0, 5, true)},
    {
        ranged_string(test[8], 0, 8, true)},
    {
        ranged_string(test[9], 3, 13, true),
        ranged_string(test[9], 14, 22, true),
        ranged_string(test[9], 22, 29, true),
        ranged_string(test[9], 37, 50, true)}} });
}
void MyUnitTest::test6() {
    int t = 6;
    static vector<string> test({
        "ac",
        "aBc",
        "aBBc",
        "aBBBc",
        "aBBBBc",
        "aBBBBBc",
        "gaBBBBBBc",
        "ggaBBBBBBBc",
        "aBBBBBBBBc",
        "aBBBBBBBBBc",
        "aBBBBBBBBBBcg",
        "aBBBBBBBBBBBcgg",
        "aBBBBBBBBBBBBc",
        "aBBBBBBBBBBBBBc",
        "kaBBBcfaBBBBckkaBBckaBBBBBBcUUUaBBBBcOOOaBcTTTTaBBBBBcTTTTT" });

    tests.push_back({
        t, "aB{3,6}c", test,
    {{ }, { }, { },
    {
        ranged_string(test[3], 0, 5, true)},
    {
        ranged_string(test[4], 0, 6, true)},
    {
        ranged_string(test[5], 0, 7, true)},
    {
        ranged_string(test[6], 1, 9, true)},
    { }, { }, { }, { }, { }, { }, { },
    {
        ranged_string(test[14], 1, 6, true),
        ranged_string(test[14], 7, 13, true),
        ranged_string(test[14], 20, 28, true),
        ranged_string(test[14], 31, 37, true),
        ranged_string(test[14], 47, 54, true)}} });
}
void MyUnitTest::test7() {
    int t = 7;
    static vector<string> test({
        "jkTT",
        "jkTTT",
        "jkTTTT",
        "jkTTTTT",
        "jkTTTTTT",
        "jkTTTTTTT" });

    tests.push_back({
        t, "jkT{4,5}", test,
    {{ }, { },
    {
        ranged_string(test[2], 0, 6, true)},
    {
        ranged_string(test[3], 0, 7, true)},
    {
        ranged_string(test[4], 0, 7, true)},
    {
        ranged_string(test[5], 0, 7, true)}} });
}
void MyUnitTest::test8() {
    int t = 8;
    static vector<string> test({
        "TT",
        "TTT",
        "TTTT",
        "TTTTT",
        "TTTTTT",
        "TTTTTTT" });

    tests.push_back({
        t, "T{3,5}", test,
    {{ },
    {
        ranged_string(test[1], 0, 3, true)},
    {
        ranged_string(test[2], 0, 4, true)},
    {
        ranged_string(test[3], 0, 5, true)},
    { }, { }    } });
}
void MyUnitTest::test9() {
    int t = 9;
    static vector<string> test({
        "k",
        "T",
        "TT",
        "TTT",
        "kkTTTT",
        "TTTTT",
        "kkkkTTTTTT",
        "TTTTTTT",
        "kkkkTTTTTTz" });

    tests.push_back({
        t, "T+", test,
    {
    { },
    {
        ranged_string(test[1], 0, 1, true)},
    {
        ranged_string(test[2], 0, 2, true)},
    {
        ranged_string(test[3], 0, 3, true)},
    {
        ranged_string(test[4], 2, 6, true)},
    {
        ranged_string(test[5], 0, 5, true)},
    {
        ranged_string(test[6], 4, 10, true)},
    {
        ranged_string(test[7], 0, 7, true)},
    {
        ranged_string(test[8], 4, 10, true)}} });;
}



//**************************  Reader Test  **********************************

void MyUnitTest::reader_test(const char* filename) {
    TestDataFileReader reader;
    bool result = reader.read(filename, tests);
    cout << filename << " 파일을 ";
    if (result) {
        cout << "성공적으로 읽었습니다. \n";
    }
    else
        cout << "읽기에 실패했습니다. \n";

    more_details_all();
    run_tests();
}

} //end of namespace assignment1