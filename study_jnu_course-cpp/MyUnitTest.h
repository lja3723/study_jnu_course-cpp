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
    class TestDataFileReader;
    struct Test {               //test
        int number;             //test case number
        string regex;           //regex of test
        vector<string> test;    //input strings for regex
        vector<vector<ranged_string>> expect; //expect results
        bool details;           //0: less detail, 1: more detail
        bool enabled;           //0: test disabled, 1: test enabled
    }; 
    class TestDataFileReader;


    vector<Test> tests;         //�׽�Ʈ ���
    int m_newline;              //������ �ʿ��� �� ���� ���� ��
    char m_underline_marker;    //���� ǥ���� ����
    string m_indent;            //�鿩���� ����
    int details_summary_mode;   //�׽�Ʈ ��� ǥ�ÿ���
    bool m_is_file_open;
    bool m_is_file_syntax_error;


    void print_run_title();
    void print_test_title(string label_left, int test_number, string label_right);
    void print_match_range(string title, vector<ranged_string>& match_result, int test_case, int elem);
    void print_match_underline(const vector<ranged_string>& result);
    void print_successed_test(vector<int>& successed_list, int test_number);
    void print_failed_test(vector<int>& failed_list, vector<bool>& results, int test_number);
    void print_tests_summary(set<int>& disabled, vector<int>& successed_list, vector<int>& failed_list);
    void clear_tests();
    void file_open(const char* name);


    //���� �׽�Ʈ ���� �Լ�
    void run_tests();
    vector<bool> run_test(int t);
    bool assertEqual(vector<ranged_string>& expect, vector<ranged_string>& result);


public:
    /**********     file open and read test     **********/
    MyUnitTest(string& filename) : MyUnitTest(filename.c_str()) {}
    MyUnitTest(const char* test_filename) :
        m_newline(80),
        m_underline_marker('^'),
        m_is_file_syntax_error(false) {
        set_indent_level(3);
        set_summary_more_details();
        clear_tests();
        file_open(test_filename);
        enable_all().more_details_all();
    }
    bool is_open();
    void run();


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


//������ �׽�Ʈ�� ����Ѵ�.
protected:

    //������ �׽�Ʈ ���    
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


class MyUnitTest::TestDataFileReader : public MyFileReader<vector<MyUnitTest::Test>> {
private:
    using Test = MyUnitTest::Test;
    using Data = vector<Test>;
    using Tokens = vector<string>;
    enum _state {   //�� 8���� ����
        init,       //���� ���� ����
        regex,      //regex:
        get_cases,  //test_cases:
        get_case,   //case:
        input,      //input:
        output,     //output: or range:
        range,      //number of range
        unknown
    } prev_state, line_means;

    const string syntax_regex;
    const string syntax_regex_ex;
    const string syntax_get_cases;
    const string syntax_get_case;
    const string syntax_input;
    const string syntax_input_ex;
    const string syntax_output1;
    const string syntax_output2;
    const string syntax_range;
    const string syntax_range_null;

    Test cur_test;
    int cur_test_num;
    vector<ranged_string> cur_expect;

    bool is_range_null;
    bool is_file_end;
    bool is_file_end_unexpected;


    bool prev_state_init(Data& container) {
        if (is_file_end) {
            if (!m_unexcepted_file_end)
                cout << "���Ͽ��� �о�� ������ �����ϴ�. �׽�Ʈ�� ���������� ������� ���� �� �ֽ��ϴ�." << endl;
            return true;
        }

        if (line_means == regex)
            return action_state_regex(container);

        return error_syntax({ syntax_regex_ex });
    }
    bool prev_state_regex(Data& container) {
        if (is_file_end)
            return error_unexpected_file_end();

        if (line_means == get_cases || line_means == get_case) {
            prev_state = line_means;
            return true;
        }

        return error_syntax({
            syntax_get_cases,
            syntax_get_case
            });
    }
    bool prev_state_get_cases(Data& container) {
        if (is_file_end)
            return error_unexpected_file_end();

        if (line_means == get_case) {
            prev_state = get_case;
            return true;
        }

        return error_syntax({ syntax_get_case });
    }
    bool prev_state_get_case(Data& container) {
        if (is_file_end)
            return error_unexpected_file_end();

        if (line_means == input) {
            if (m_tokens.size() == 1)
                return error_argument_missing({ syntax_input_ex });

            cur_test.test.push_back(m_tokens[1]);
            prev_state = input;
            return true;
        }

        return error_syntax({ syntax_input_ex });
    }
    bool prev_state_input(Data& container) {
        if (is_file_end)
            return error_unexpected_file_end();

        if (line_means == output) {
            prev_state = output;
            return true;
        }

        return error_syntax({ syntax_output1, syntax_output2 });
    }
    bool prev_state_output(Data& container) {
        if (is_file_end)
            return error_unexpected_file_end();

        if (line_means == unknown && lowercase(m_tokens[0]) == "null") {
            is_range_null = true;

            cur_test.expect.push_back(vector<ranged_string>());
            prev_state = range;

            return true;
        }

        if (line_means == range)
            return action_state_range({ syntax_range, syntax_range_null });

        return error_syntax({ syntax_range, syntax_range_null });
    }
    bool prev_state_range(Data& container) {
        if (is_file_end) {
            push_and_clear_cur_expect(container);
            container.push_back(cur_test);
            return true;
        }

        if (line_means == range) {
            if (is_range_null)
                return error_syntax({
                    syntax_regex_ex,
                    syntax_get_case
                    });

            is_range_null = false;
            return action_state_range({ syntax_range });
        }

        if (line_means == get_case) {
            if (!is_range_null)
                push_and_clear_cur_expect(container);

            is_range_null = false;
            prev_state = get_case;
            return true;
        }

        if (line_means == regex)
            return action_state_regex(container);


        return error_syntax({
            syntax_regex_ex,
            syntax_get_case,
            syntax_range
            });
    }
    void push_and_clear_cur_expect(Data& container) {
        cur_test.expect.push_back(cur_expect);
        cur_expect.clear();
    }
    bool action_state_regex(Data& container) {
        if (m_tokens.size() <= 1)
            return error_argument_missing({ syntax_regex_ex });

        if (cur_test_num > 0) {
            push_and_clear_cur_expect(container);
            container.push_back(cur_test);
        }
        init_cur_test(++cur_test_num);
        cur_test.regex = m_tokens[1];

        is_range_null = false;
        prev_state = regex;
        return true;
    }
    bool action_state_range(const vector<string>& expects) {
        if (m_tokens.size() == 1)
            return error_argument_missing(expects);

        if (!is_digit(m_tokens[1]))
            return error_invalid_digit(expects);

        cur_expect.push_back(ranged_string(
            cur_test.test.back(),
            stoi(m_tokens[0]), stoi(m_tokens[1]),
            true));

        prev_state = range;
        return true;
    }


    _state interpret(const string& str) {
        if (lowercase(str) == syntax_regex) return regex;
        if (lowercase(str) == syntax_get_cases) return get_cases;
        if (lowercase(str) == syntax_get_case) return get_case;
        if (lowercase(str) == syntax_input) return input;
        if (lowercase(str) == syntax_output1) return output;
        if (lowercase(str) == syntax_output2) return output;
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
        cout << "[error] line " << m_cur_line << " of \"" << get_filename() << "\": ";
    }
    void show_expected_syntax(const vector<string>& expects) {
        if (expects.empty()) return;

        string indent = "    ";
        cout << "���� ������ ������ ���ҽ��ϴ�." << endl;
        cout << indent << indent << expects.front() << endl;

        for (int i = 1; i < expects.size(); i++)
            cout << indent << "or  " << expects[i] << endl;
    }
    void init_cur_test(int test_num) {
        cur_test.number = test_num;
        cur_test.regex.clear();
        cur_test.test.clear();
        cur_test.expect.clear();
        cur_test.details = false;
        cur_test.enabled = true;
    }


    bool error_unexpected_file_end() {
        if (!m_unexcepted_file_end) {
            showline();
            cout << "����ġ ���ϰ� ������ ����Ǿ����ϴ�." << endl;
        }
        return false;
    }
    bool error_argument_missing(const vector<string>& expects) {
        showline();
        cout << "�־��� ���ڰ� ��밪���� �����ϴ�." << endl;
        show_expected_syntax(expects);
        return false;
    }
    bool error_invalid_digit(const vector<string>& expects) {
        showline();
        cout << "�߸��� ���� ����Դϴ�." << endl;
        show_expected_syntax(expects);
        return false;
    }
    bool error_syntax(const vector<string>& expects) {
        showline();
        cout << "�߸��� �����Դϴ�." << endl;
        show_expected_syntax(expects);
        return false;
    }


protected:
    virtual bool single_line_action(Data& container) override {
        is_file_end = false;
        return action(container);
    }
    virtual bool end_file_action(Data& container) override {
        is_file_end = true;
        line_means = unknown;
        return action(container);
    }
    bool action(Data& container) {
        if (!is_file_end)
            line_means = interpret(m_tokens[0]);

        switch (prev_state) {
        case init:      return prev_state_init(container);
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
        prev_state(init), line_means(init),
        cur_test_num(0), is_range_null(false),
        is_file_end(false), is_file_end_unexpected(false),
        syntax_regex("regex:"),
        syntax_regex_ex(syntax_regex + " {regex for test}"),
        syntax_get_cases("test_cases:"),
        syntax_get_case("case:"),
        syntax_input("input:"),
        syntax_input_ex(syntax_input + " {your test string}"),
        syntax_output1("output:"),
        syntax_output2("range:"),
        syntax_range("{range_start_number} {range_end_number}"),
        syntax_range_null("NULL\t(no range expected)") {
        init_cur_test(0);
    }
};


} //end of namespace assignment1
#endif
