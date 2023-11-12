#ifndef __MY_UNIT_TEST_H__
#define __MY_UNIT_TEST_H__
#include <iostream>
#include <vector>
#include <string>
#include <set>

#include "MySimpleRegex.h"
#include "DataReader.h"

namespace assignment1 {
using namespace std;


class MyUnitTest {
    friend class TestDataFileReader;
private:
    struct Test {               //test
        int number;             //test case number
        string regex;           //regex of test
        vector<string> test;    //input strings for regex
        vector<vector<ranged_string>> expect; //expect results
        bool details;           //0: less detail, 1: more detail
        bool enabled;           //0: test disabled, 1: test enabled
    }; 

    vector<Test> tests;         //테스트 목록
    int m_newline;              //개행이 필요할 때 기준 문자 수
    char m_underline_marker;    //밑줄 표시할 글자
    string m_indent;            //들여쓰기 문자
    int details_summary_mode;   //테스트 요약 표시여부

    class TestDataFileReader;

    void print_run_title();
    void print_test_title(string label_left, int test_number, string label_right);
    void print_match_range(string title, vector<ranged_string>& match_result, int test_case, int elem);
    void print_match_underline(const vector<ranged_string>& result);
    void print_successed_test(vector<int>& successed_list, int test_number);
    void print_failed_test(vector<int>& failed_list, vector<bool>& results, int test_number);
    void print_tests_summary(set<int>& disabled, vector<int>& successed_list, vector<int>& failed_list);


    //실제 테스트 수행 함수
    void run_tests();
    vector<bool> run_test(int t);
    bool assertEqual(vector<ranged_string>& expect, vector<ranged_string>& result);


public:
    MyUnitTest() :
        m_newline(80),
        m_underline_marker('^') {
        set_indent_level(3);
        set_summary_more_details();

        register_tests();
        enable_all().more_details_all();
    }
    void run() { run_tests(); }


    /**********     Test Options     **********/
    MyUnitTest& set_newline(int newline) {
        m_newline = newline;
        return *this;
    }
    MyUnitTest& set_underline_marker(char marker) {
        m_underline_marker = marker;
        return *this;
    }
    MyUnitTest& set_indent_level(int indent_level) {
        m_indent = "";
        for (int i = 0; i < indent_level; i++) m_indent += " ";
        return *this;
    }
    MyUnitTest& set_summary_more_details() {
        details_summary_mode = true;
        return *this;
    }
    MyUnitTest& set_summary_less_details() {
        details_summary_mode = false;
        return *this;
    }

    MyUnitTest& more_details(int test) { return more_details({ test }); }
    MyUnitTest& more_details(initializer_list<int> tests_list) {
        for (int test_num : tests_list) tests[test_num].details = true;
        return *this;
    }
    MyUnitTest& less_details(int test) { return less_details({ test }); }
    MyUnitTest& less_details(initializer_list<int> tests_list) {
        for (int test_num : tests_list) tests[test_num].details = false;
        return *this;
    }
    MyUnitTest& more_details_all() {
        for (Test& t : tests) t.details = true;
        return *this;
    }
    MyUnitTest& less_details_all() {
        for (Test& t : tests) t.details = false;
        return *this;
    }

    MyUnitTest& enable(int test) { return enable({ test }); }
    MyUnitTest& enable(initializer_list<int> tests_list) {
        cout << "initializer_list" << endl;
        for (int test_num : tests_list) tests[test_num].enabled = true;
        return *this;
    }
    MyUnitTest& disable(int test) { return disable({ test }); }
    MyUnitTest& disable(initializer_list<int> tests_list) {
        for (int test_num : tests_list) tests[test_num].enabled = false;
        return *this;
    }
    MyUnitTest& enable_all() {
        for (Test& t : tests) t.enabled = true;
        return *this;
    }
    MyUnitTest& disable_all() {
        for (Test& t : tests) t.enabled = false;
        return *this;
    }

    /*debug*/ void reader_test(const char* filename);


//실행할 테스트를 등록한다.
protected:
    void register_tests() {
        test0(); //register example (<!> do not delete <!>)
        //test1(); test2(); test3(); test4(); test5();
        //test6(); test7(); test8(); test9();
    }

    //test func example
    void test0() {
        int t = 0;
        vector<string> test({
            "test_str",
            "test_str" });

        tests.push_back({
            t, "test_regex", test,
        {{
            ranged_string(test[0], 0, 0, true)},
        {
            ranged_string(test[1], 0, 0, true)}} });
    }


    //실행할 테스트 등록    
    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
    void test9();
};


//TODO: range의 ref에 NULL이 들어가는 문제가 있음
//해결해야 됨
class MyUnitTest::TestDataFileReader : public MyFileReader<vector<MyUnitTest::Test>> {
private:
    using Test = MyUnitTest::Test;
    using Data = vector<Test>;
    using Tokens = vector<string>;

    enum _state {   //총 8가지 상태
        regex,      //regex:
        get_cases,  //test_cases:
        get_case,   //case:
        input,      //input:
        output,     //output: or range:
        range,      //number of range
        unknown     
    } prev_state, line_means;


    Test cur_test;
    int cur_test_num;
    vector<ranged_string> cur_test_expect;
    bool is_range_null;
    bool is_file_end;


    bool prev_state_regex(Data& container) {
        if (is_file_end) return error_file_end();

        if (line_means == get_cases || line_means == get_case) {
            prev_state = line_means;
            return true;
        }

        return error_syntax({
            "test_cases:",
            "case:"
        });
    }

    bool prev_state_get_cases(Data& container) {
        if (is_file_end) return error_file_end();

        if (line_means == get_case) {
            prev_state = get_case;
            return true;
        }

        return error_syntax({ "case:" });
    }

    bool prev_state_get_case(Data& container) {
        if (is_file_end) return error_file_end();

        if (line_means == input) {
            if (m_tokens.size() == 1)
                return error_argument_missing();

            cur_test.test.push_back(m_tokens[1]);
            prev_state = input;
            return true;
        }

        return error_syntax({ "input: {your test string}" });
    }

    bool prev_state_input(Data& container) {
        if (is_file_end) return error_file_end();

        if (line_means == output) {
            prev_state = output;
            return true;
        }

        return error_syntax({ "output:", "range:" });
    }

    bool prev_state_output(Data& container) {
        if (is_file_end) return error_file_end();

        if (line_means == unknown) {
            if (lowercase(m_tokens[0]) == "null") {
                is_range_null = true;

                cur_test.expect.push_back(vector<ranged_string>());
                prev_state = range;

                return true;
            }
            else return error_syntax({
                "{range_start_number} {range_end_number}",
                "NULL\t(no range expected)"
            });
        }

        if (line_means == range)
            return add_range();

        return true;
    }

    bool prev_state_range(Data& container) {
        if (is_file_end) {
            if (cur_test.number > 0) {
                expect_push_and_clear(container);
                container.push_back(cur_test);
            }
            return true;
        }

        if (line_means == range) {
            if (is_range_null)
                return error_syntax({
                    "regex: {regex for test}",
                    "case:"
                });

            return add_range();
        }

        if (line_means == regex) {
            if (m_tokens.size() <= 1)
                return error_argument_missing();
            
            if (cur_test.number > 0) {
                expect_push_and_clear(container);
                container.push_back(cur_test);
            }

            init_cur_test(++cur_test_num);
            cur_test.regex = m_tokens[1];

            is_range_null = false;
            prev_state = regex;
            return true;
        }

        if (line_means == get_case) {
            if (!is_range_null) 
                expect_push_and_clear(container);

            is_range_null = false;
            prev_state = get_case;
            return true;
        }

        return error_syntax({ 
            "regex: {regex for test}",
            "case:",
            "{range_start_number} {range_end_number}"
        });
    }
    
    void expect_push_and_clear(Data& container) {
        cur_test.expect.push_back(cur_test_expect);
        cur_test_expect.clear();
    }

    bool add_range() {
        if (m_tokens.size() == 1)
            return error_argument_missing();

        if (!is_digit(m_tokens[1]))
            return error_invalid_digit();

        unsigned the_size = cur_test.test.size();
        string& ref = cur_test.test[the_size - 1];
        cur_test_expect.push_back(ranged_string(
            ref, stoi(m_tokens[0]), stoi(m_tokens[1]), true));

        prev_state = range;
        return true;
    }



    _state interpret(const string& str) {
        if (lowercase(str) == "regex:") return regex;
        if (lowercase(str) == "test_cases:") return get_cases;
        if (lowercase(str) == "case:") return get_case;
        if (lowercase(str) == "input:") return input;
        if (lowercase(str) == "output:") return output;
        if (lowercase(str) == "range:") return output;
        if (is_digit(str)) return range;
        return unknown;
    }

    string lowercase(const string& str) {
        string ret = str;
        for (char& c : ret) if ('A' <= c && c <= 'Z') c += 'a' - 'A';
        return ret;
    }

    bool is_digit(const string& s) {
        bool ret = true;
        for (char c : s) ret &= ('0' <= c && c <= '9');
        return ret;
    }

    void showline() {
        cout << "[error] line " << m_cur_line << " of file \"" << get_filename() << "\": ";
    }

    bool error_file_end() {
        showline();
        cout << "예기치 못하게 파일이 종료되었습니다." << endl;
        return false;
    }

    bool error_argument_missing() {
        showline();
        cout << "주어진 인자가 너무 적습니다." << endl;
        return false;
    }

    bool error_invalid_digit() {
        showline();
        cout << "잘못된 숫자 양식입니다." << endl;
        return false;
    }

    bool error_syntax(const vector<string>& expects) {
        showline();
        cout << "잘못된 구문입니다." << endl;
        cout << "기대된 구문은 다음과 같았습니다." << endl;
        for (int i = 0; i + 1 < expects.size(); i++)
            cout << "\t" << expects[i] << "\tor" << endl;
        cout << "\t" << expects.back() << endl;
        return false;
    }

    void init_cur_test(int test_num) {
        cur_test.number = test_num;
        cur_test.regex.clear();
        cur_test.test.clear();
        cur_test.expect.clear();
        cur_test.details = false;
        cur_test.enabled = true;
    }

protected:
    virtual bool single_line_action(Data& container) override {
        is_file_end = false;
        return action(container);
    }

    virtual bool end_file_action(Data& container) override {
        cout << "파일의 끝에 도달했습니다." << endl;
        is_file_end = true;
        line_means = unknown;
        return action(container);
    }

    bool action(Data& container) {
        if (!is_file_end)
            line_means = interpret(m_tokens[0]);         

        switch (prev_state) {
        case regex:     return prev_state_regex(container);
        case get_cases: return prev_state_get_cases(container);
        case get_case:  return prev_state_get_case(container);
        case input:     return prev_state_input(container);
        case output:    return prev_state_output(container);
        case range:     return prev_state_range(container);
        }

        cout << "TestDataFileReader error: unknown state occured" << endl;
        return false;
    }

public:
    TestDataFileReader() : 
        prev_state(range), line_means(unknown), 
        cur_test_num(0), is_file_end(false), is_range_null(false) {
        init_cur_test(0);
    }
};


} //end of namespace assignment1
#endif