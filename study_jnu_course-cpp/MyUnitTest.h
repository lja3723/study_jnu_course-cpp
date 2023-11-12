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
    struct test {               //test
        int number;             //test case number
        string regex;           //regex of test
        vector<string> test;    //input strings for regex
        vector<vector<ranged_string>> expect; //expect results
        bool details;           //0: less detail, 1: more detail
        bool enabled;           //0: test disabled, 1: test enabled
    };

    vector<test> tests;         //테스트 목록
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
        for (test& t : tests) t.details = true;
        return *this;
    }
    MyUnitTest& less_details_all() {
        for (test& t : tests) t.details = false;
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
        for (test& t : tests) t.enabled = true;
        return *this;
    }
    MyUnitTest& disable_all() {
        for (test& t : tests) t.enabled = false;
        return *this;
    }

    /*debug*/ void reader_test(const char* filename);


//실행할 테스트를 등록한다.
protected:
    void register_tests() {
        test0(); //register example (<!> do not delete <!>)
        test1(); test2(); test3(); test4(); test5();
        test6(); test7(); test8(); test9();
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


class MyUnitTest::TestDataFileReader : public MyFileReader<vector<MyUnitTest::test>> {
protected:
    using Data = vector<MyUnitTest::test>;
    using Tokens = vector<string>;

    enum _state {
        test,
        regex,
        get_cases,
        get_case,
        input,
        output,
        range,
        unknown //총 8가지 상태
    } cur_state;

    _state interpret(const string& str) {
        if (str == "test_no:") return test;
        if (str == "regex:") return regex;
        if (str == "test_cases") return get_cases;
        if (str == "case:") return get_case;
        if (str == "input:") return input;
        if (str == "output:") return output;
        if (is_digit(str)) return range;
        return unknown;
    }

    bool is_digit(const string& s) {
        bool ret = true;
        for (char c : s) ret &= ('0' <= c && c <= '9');
        return ret;
    }

    bool state_test(Data& container, Tokens& tokens, int cur_line) {
        return true;
    }

    bool state_regex(Data& container, Tokens& tokens, int cur_line) {
        return true;
    }

    bool state_get_cases(Data& container, Tokens& tokens, int cur_line) {
        return true;
    }

    bool state_get_case(Data& container, Tokens& tokens, int cur_line) {
        return true;
    }

    bool state_input(Data& container, Tokens& tokens, int cur_line) {
        return true;
    }

    bool state_output(Data& container, Tokens& tokens, int cur_line) {
        return true;
    }

    bool state_range(Data& container, Tokens& tokens, int cur_line) {
        return true;
    }

    virtual bool single_line_action(Data& container, Tokens& tokens, int cur_line) {
        switch (cur_state) {
        case test: return state_test(container, tokens, cur_line);
        case unknown: return false;
            
        }

        return true;
    }

    virtual bool end_file_action(Data& container, bool read_successed) {
        cout << "파일의 끝에 도달했습니다." << endl;

        return true;
    }

public:
    TestDataFileReader()
        : cur_state(test)
    {
         
    }
};


} //end of namespace assignment1
#endif