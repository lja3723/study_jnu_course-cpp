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


/*
class MyUnitTest:
MySimpleRegex의 동작이 올바른지 자동으로 테스트한다.

테스트 데이터 txt 파일에서 테스트할 케이스를 불러온다.
    생성자 MyUnitTest(파일이름) 호출로 파일을 오픈한다.
    is_open() : 파일이 정상적으로 열렸는지 확인한다.
    run() : 읽어온 데이터에 대해 테스트를 수행한다.
     
테스트가 종료되면 요약 결과를 출력한다.
    set_summary_more_details() : 요약 결과를 상세히 출력한다.
 
옵션을 다르게 주어 자세한 테스트 결과 출력할 수 있다. 
    more_details_all() : 모든 테스트 결과를 자세히 출력한다.
    less_details_all() : 모든 테스트 결과를 간략히 출력한다.

옵션을 다르게 주어 단위 테스트 수행을 각각 활성화하거나 비활성화 할 수 있다.
    enable_all() : 모든 테스트를 수행(활성화)한다.
    disable_all() : 모든 테스트를 비활성화한다. 수행하지 않는다.
 */
class MyUnitTest {
    friend class TestDataFileReader; //테스트 데이터 파일 리더기
    using ranges = vector<ranged_string>;

    struct Test {               //단위 테스트
        int number;             //test case number
        string regex;           //regex of test
        vector<string> test;    //input strings for regex
        vector<ranges> expect; //expect results
        bool details = false;   //0: less detail, 1: more detail
        bool enabled = true;    //0: test disabled, 1: test enabled
    };


/*************************************/
/*      테스트 실행 및 관련 유틸 함수   */
/*      실행 순서대로 배치되었음        */
/*************************************/
public:
    //생성자 호출 동시에 파일이름 제공 필요
    MyUnitTest(const char* test_filename); //생성 시 테스트파일 이름 필요
    MyUnitTest(string& filename) : MyUnitTest(filename.c_str()) {}

    //테스트 실행 명령
    bool is_open();
    void run();

private:
    //단위 테스트 위한 초기화 과정 및 파일 열기
    void clear_tests();
    void file_open(const char* name); //파일을 열어야함

    // 테스트 실제 수행 함수
    void run_tests(); //모든 테스트 케이스 수행
    vector<bool> run_test(const Test& t); //각각의 단위 테스트 수행
    bool assertEqual(const ranges& expect, const ranges& result);
/****************************************************/



public:
    /**********     테스트 옵션     **********/
    MyUnitTest& set_newline(int newline);
    MyUnitTest& set_underline_marker(char marker);
    MyUnitTest& set_indent_level(int indent_level);
    MyUnitTest& set_summary_more_details();
    MyUnitTest& set_summary_less_details();

    MyUnitTest& more_details(int test);
    MyUnitTest& more_details(initializer_list<int> tests_list);
    MyUnitTest& less_details(int test);
    MyUnitTest& less_details(initializer_list<int> tests_list);

    MyUnitTest& enable(int test);
    MyUnitTest& enable(initializer_list<int> tests_list);
    MyUnitTest& disable(int test);
    MyUnitTest& disable(initializer_list<int> tests_list);

    MyUnitTest& more_details_all();
    MyUnitTest& less_details_all();
    MyUnitTest& enable_all();
    MyUnitTest& disable_all();



private:
    /***************  출력 함수  ***************/
    void print_run_title();
    void print_test_title(string label_left, const Test& t, string label_right);
    void print_match_range(string title, const ranges& match_result, const Test& t, int elem);
    void print_match_underline(const ranges& result);
    void print_successed_test(const Test& t);
    void print_failed_test(const Test& t, vector<bool>& results);
    void print_tests_summary(set<int>& disabled, vector<int>& successed_list, vector<int>& failed_list);


    /**********   private fields   **********/
    class TestDataFileReader;   //테스트데이터 파일 리더

    vector<Test> tests;         //테스트 목록
    int m_newline;              //개행이 필요할 때 기준 문자 수
    char m_underline_marker;    //밑줄 표시할 글자
    string m_indent;            //들여쓰기 문자
    int details_summary_mode;   //테스트 요약 표시여부
    bool m_is_file_open;        //파일 열림 여부
    bool m_no_syntax_error;     //파일 문법 올바름 여부


};



//테스트 데이터 txt 파일을 읽어 단위 테스트 객체 배열을 생성한다.
class MyUnitTest::TestDataFileReader 
    : public MyFileReader<vector<MyUnitTest::Test>> {
private:
    using Test = MyUnitTest::Test;
    using Data = vector<Test>;
    using Tokens = vector<string>;
    enum _state {   //총 8가지 상태
        init,       //파일 최초 오픈
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



    /****************  상태 함수  ****************/
    bool prev_state_init(Data& container) {
        if (is_file_end) {
            if (!m_unexcepted_file_end)
                cout << "파일에서 읽어온 내용이 없습니다. 테스트가 정상적으로 수행되지 않을 수 있습니다." << endl;
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

    /****************  유틸 함수  ****************/
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
        cout << "기대된 구문은 다음과 같았습니다." << endl;
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

    /****************  오류 함수  ****************/
    bool error_unexpected_file_end() {
        if (!m_unexcepted_file_end) {
            showline();
            cout << "예기치 못하게 파일이 종료되었습니다." << endl;
        }
        return false;
    }
    bool error_argument_missing(const vector<string>& expects) {
        showline();
        cout << "주어진 인자가 기대값보다 적습니다." << endl;
        show_expected_syntax(expects);
        return false;
    }
    bool error_invalid_digit(const vector<string>& expects) {
        showline();
        cout << "잘못된 숫자 양식입니다." << endl;
        show_expected_syntax(expects);
        return false;
    }
    bool error_syntax(const vector<string>& expects) {
        showline();
        cout << "잘못된 구문입니다." << endl;
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
