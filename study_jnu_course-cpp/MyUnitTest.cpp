#include "MyUnitTest.h"

namespace assignment1 {
	

/*************************************/
/*      �׽�Ʈ ���� �� ���� ��ƿ �Լ�   */
/*      ���� ������� ��ġ�Ǿ���        */
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
    tests.push_back({ 0, "", vector<string>(), vector<vector<ranged_string>>() });


}
void MyUnitTest::file_open(const char* name) {
    TestDataFileReader reader;
    m_is_file_open = reader.open(name);
    if (!m_is_file_open) return;

    m_no_syntax_error = reader.read(tests);
    if (m_no_syntax_error)
        cout << "\"" << name << "\" ������ ���������� �о����ϴ�." << endl;


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

    //��Ȱ��ȭ�� �׽�Ʈ ����Ʈ ���ϱ�
    set<int> disabled;
    for (int t = 1; t < tests.size(); t++)
        if (!tests[t].enabled) disabled.insert(tests[t].number);

    //�׽�Ʈ ��� ��� ���� ��� ��������
    vector<int> successed_list;
    vector<int> failed_list;

    //�� �׽�Ʈ ���̽��� ���� �׽�Ʈ ����
    for (int t = 1; t < tests.size(); t++) {
        //disabled�� �׽�Ʈ ��ŵ
        if (disabled.find(t) != disabled.end()) continue;

        //�׽�Ʈ ����
        vector<bool> results = run_test(tests[t].number);

        //�׽�Ʈ �������� ���
        bool test_successed = true;
        for (bool result : results) test_successed &= result;

        if (test_successed) {
            successed_list.push_back(t);
            print_successed_test(t);
        }
        else {
            failed_list.push_back(t);
            print_failed_test(t, results);
        }
    }

    //�׽�Ʈ ���� ��� ���
    print_tests_summary(disabled, successed_list, failed_list);


}
vector<bool> MyUnitTest::run_test(int t) {
    vector<bool> ret;

    //���Խ��� �������Ѵ�.
    MySimpleRegex::compiled cp 
        = MySimpleRegex::compile(tests[t].regex);

    //�� ���̽� ���ڿ����� ��밪(expect)�� ���ప(result)�� ������ ���Ѵ�.
    for (int i = 0; i < tests[t].test.size(); i++) {
        vector<ranged_string> result = cp.match_all(tests[t].test[i]);
        ret.push_back(assertEqual(tests[t].expect[i], result));
    }

    return ret;


}
bool MyUnitTest::assertEqual(vector<ranged_string>& expect, vector<ranged_string>& result) {
    if (expect.size() != result.size()) return false;

    for (int i = 0; i < expect.size(); i++) {
        if (expect[i].start != result[i].start) return false;
        if (expect[i].end != result[i].end)     return false;
        if (expect[i].is_valid != result[i].is_valid) return false;
    }
    return true;


}



/*******************   �׽�Ʈ �ɼ�   *******************/
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



/*******************   ��� �Լ�   *******************/
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
        cout << m_indent << tests[test_case].test[elem] << " -> ��ġ���� ����" << "\n\n";
        return;
    }

    print_match_underline(match_result);
    for (const ranged_string& mt : match_result) {
        cout << m_indent << m_indent;
        cout << "-> " << mt << endl;
    }
    cout << endl;


}
void MyUnitTest::print_match_underline(const vector<ranged_string>& result) {
    //��ġ�� ������ ���� ǥ��; �Է� ���ڿ��� ��� newline���� �����
    if (result.empty()) return;

    const string& ref = result[0].ref;

    //���ڿ��� ������ ��� ���
    for (size_t i = 0, cur_match = 0; i < ref.size(); i += m_newline) {
        size_t len = i + m_newline <= ref.size() ? m_newline : ref.size() - i;
        cout << m_indent << ref.substr(i, len) << endl; //���� ���ڿ� ���  
        if (cur_match >= result.size() || i + len <= result[cur_match].start) {
            cout << endl;
            continue;
        }

        cout << m_indent;
        for (size_t j = i; j < i + len; j++) {
            const ranged_string& str = result[cur_match];
            cout << (str.start <= j && j < str.end ? m_underline_marker : ' '); // ���� ���
            if (j + 1 >= str.end) cur_match++;
            if (cur_match >= result.size()) break;
        }
        cout << endl;
    }


}
void MyUnitTest::print_successed_test(int test_number) {
    if (!tests[test_number].details) {
        cout << "�׽�Ʈ #" << test_number << " ���" << endl;
        return;
    }

    print_test_title("�׽�Ʈ #", test_number, " ��� (more detail mode enabled)");
    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex);

    for (int i = 0; i < tests[test_number].test.size(); i++) {
        vector<ranged_string> result = cp.match_all(tests[test_number].test[i]);
        cout << "case #" << (i + 1) << endl;
        print_match_range("< ������ >", result, test_number, i);
    }
    cout << "\n\n";


}
void MyUnitTest::print_failed_test(int test_number, vector<bool>& results) {
    //������ �׽�Ʈ�� �����ϱ� ���� ���Խ� ��������
    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex);

    bool title_printed = false;
    for (int i = 0; i < results.size(); i++) {
        if (!tests[test_number].details && results[i] == true) continue;
        vector<ranged_string> result = cp.match_all(tests[test_number].test[i]);

        //������ �׽�Ʈ ���� ��� (���� 1ȸ��)
        if (!title_printed) {
            title_printed = true;
            string label_right = "����";
            label_right += tests[i].details ? " (more detail mode enabled)" : "";
            print_test_title("<!>>>>>>> �׽�Ʈ #", test_number, label_right);
        }

        //������ ���̽� ���
        cout << "case #" << (i + 1) << (results[i] ? " (���)" : " (����)") << endl;
        if (!results[i])
            print_match_range("< ����� >", tests[test_number].expect[i], test_number, i);
        print_match_range("< ������ >", result, test_number, i);
    }
    cout << "\n\n";


}
void MyUnitTest::print_tests_summary(set<int>& disabled, vector<int>& successed_list, vector<int>& failed_list) {
    cout << "\n\n";
    if (details_summary_mode) {
        string lines = "=========================";
        size_t tests_size = tests.size() - 1;
        cout << lines << "  [�׽�Ʈ ���� ���]  " << lines << endl;
        cout << "�� �׽�Ʈ���̽� ��:\t" << tests_size << " ��\n";
        if (!disabled.empty())
            cout << "��Ȱ��ȭ�� �׽�Ʈ ��:\t" << disabled.size() << " ��\n";
        cout << "������ �׽�Ʈ ��:\t" << (tests_size - disabled.size()) << " ��\n";
        cout << "������ �׽�Ʈ���̽� ��:\t" << (successed_list.size()) << " ��\n";
        if (!failed_list.empty())
            cout << "������ �׽�Ʈ���̽� ��:\t" << failed_list.size() << " ��\n";
        cout << endl;

        if (!disabled.empty()) {
            cout << "��Ȱ��ȭ�� �׽�Ʈ���̽� ���:" << endl << m_indent << m_indent;
            for (int test : disabled) cout << test << ";  ";
            cout << endl;
        }
        if (!disabled.empty()) {
            cout << "������ �׽�Ʈ���̽� ���:" << endl << m_indent << m_indent;
            for (int test : successed_list) cout << test << ";  ";
            cout << endl;
        }
        if (!failed_list.empty()) {
            cout << "������ �׽�Ʈ���̽� ���:" << endl << m_indent << m_indent;
            for (int test : failed_list) cout << test << ";  ";
            cout << endl;
        }
    }
    else {
        if (failed_list.empty()) {
            cout << "�׽�Ʈ�� ��� ����Ͽ����ϴ�." << endl;
        }
        else {
            cout << "������ �׽�Ʈ�� �ֽ��ϴ�." << endl << m_indent << "����Ʈ:  ";
            for (int test : failed_list) cout << test << ";  ";
            cout << endl;
        }
        if (disabled.empty()) return;
        cout << m_indent << "��Ȱ��ȭ�� �׽�Ʈ ����Ʈ:  ";
        for (int test : disabled) cout << test << ";  ";
        cout << endl;
    }

    //less_details_all�� Ȱ��ȭ���� Ȯ��
    bool less_details_all = true;
    for (int i = 1; i < tests.size(); i++)
        less_details_all &= !tests[i].details;
    if (less_details_all) {
        cout << endl;
        cout << "����ȭ�� �׽�Ʈ ��� ���Ⱑ ���� �ֽ��ϴ�." << endl;
        cout << "�� �ڼ��� �׽�Ʈ ����� ���÷��� run() ���� ������ more_details_all()�� �����ϼ���." << endl;
    }
    cout << "\n\n";
}



} //end of namespace assignment1