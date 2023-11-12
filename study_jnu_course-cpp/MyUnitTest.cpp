#include "MyUnitTest.h"

namespace assignment1 {
	

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

void MyUnitTest::print_match_range(string title, matches& match_result, int test_case, int elem) {
    cout << m_indent << title << endl;
    if (match_result.empty()) {
        cout << m_indent << tests[test_case].test[elem] << " -> ��ġ���� ����" << "\n\n";
        return;
    }

    print_match_underline(match_result);
    for (const MySimpleRegex::matched& mt : match_result) {
        cout << m_indent << m_indent;
        cout << "-> range [" << mt.start() << "," << mt.end() << ") " << "\"" << mt.group() << "\"\n";
    }
    cout << endl;
}

//��ġ�� ������ ���� ǥ��; �Է� ���ڿ��� ��� newline���� �����
void MyUnitTest::print_match_underline(const vector<MySimpleRegex::matched>& result) {
    if (result.empty()) return;
    const string& ref = result[0].ref();

    //���ڿ��� ������ ��� ���
    for (size_t i = 0, cur_match = 0; i < ref.size(); i += m_newline) {
        size_t len = i + m_newline <= ref.size() ? m_newline : ref.size() - i;
        cout << m_indent << ref.substr(i, len) << endl; //���� ���ڿ� ���  
        if (cur_match >= result.size() || i + len <= result[cur_match].start()) {
            cout << endl;
            continue;
        }

        cout << m_indent;
        for (size_t j = i; j < i + len; j++) {
            const MySimpleRegex::matched& match = result[cur_match];
            cout << (match.start() <= j && j < match.end() ? m_underline_marker : ' '); // ���� ���
            if (j + 1 >= match.end()) cur_match++;
            if (cur_match >= result.size()) break;
        }
        cout << endl;
    }
}

void MyUnitTest::print_successed_test(vector<int>& successed_list, int test_number) {
    successed_list.push_back(test_number);
    if (!tests[test_number].details) {
        cout << "�׽�Ʈ #" << test_number << " ���" << endl;
        return;
    }

    print_test_title("�׽�Ʈ #", test_number, " ��� (more detail mode enabled)");
    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex, test_number);

    for (int i = 0; i < tests[test_number].test.size(); i++) {
        vector<MySimpleRegex::matched> result = cp.match_all(tests[test_number].test[i]);
        cout << "case #" << (i + 1) << endl;
        print_match_range("< ������ >", result, test_number, i);
    }
    cout << "\n\n";
}

void MyUnitTest::print_failed_test(vector<int>& failed_list, vector<bool>& results, int test_number) {
    failed_list.push_back(test_number);
    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[test_number].regex, test_number);
    bool title_printed = false;
    for (int i = 0; i < results.size(); i++) {
        if (!tests[test_number].details && results[i] == true) continue;
        matches result = cp.match_all(tests[test_number].test[i]);

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
        string line = "=========================";
        unsigned tests_size = unsigned(tests.size() - 1);
        cout << line << "  [�׽�Ʈ ���� ���]  " << line << endl;
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



/*******************   �׽�Ʈ ���� �Լ�   *******************/
bool MyUnitTest::assertEqual(matches& expect, matches& result) {
    if (expect.size() != result.size()) return false;

    for (int i = 0; i < expect.size(); i++) {
        if (expect[i].start() != result[i].start()) return false;
        if (expect[i].end() != result[i].end())     return false;
        if (expect[i].valid() != result[i].valid()) return false;
    }
    return true;
}

vector<bool> MyUnitTest::run_test(int t) {
    vector<bool> ret;

    MySimpleRegex::compiled cp = MySimpleRegex::compile(tests[t].regex, t);
    for (int i = 0; i < tests[t].test.size(); i++) {
        matches list_all = cp.match_all(tests[t].test[i]);
        ret.push_back(assertEqual(tests[t].expect[i], list_all));
    }

    return ret;
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
        if (disabled.find(t) != disabled.end()) continue; //disabled�� �׽�Ʈ ��ŵ
        vector<bool> results = run_test(tests[t].number); //�׽�Ʈ ����

        //�׽�Ʈ ��������
        bool test_successed = true;
        for (bool result : results) test_successed &= result;

        if (test_successed)
            print_successed_test(successed_list, t);
        else
            print_failed_test(failed_list, results, t);
    }

    //�׽�Ʈ ���� ��� ���
    print_tests_summary(disabled, successed_list, failed_list);
}



/*******************   �׽�Ʈ �����Լ�   *******************/
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
        match(test[0], 0, 3, true)},
    {
        match(test[1], 0, 3, true) },
    {
        match(test[2], 9, 12, true),
        match(test[2], 17, 20, true),
        match(test[2], 24, 27, true) },
    { },
    {
        match(test[4], 7, 10, true),
        match(test[4], 18, 21, true), },
    {
        match(test[5], 20, 23, true) },
    {
        match(test[6], 23, 26, true),
        match(test[6], 27, 30, true),
        match(test[6], 100, 103, true),
        match(test[6], 107, 110, true)}} });
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
        match(test[0], 0, 3, true)},
    { },
    {
        match(test[2], 1, 4, true)},
    {
        match(test[3], 0, 3, true),
        match(test[3], 4, 7, true),
        match(test[3], 8, 11, true),
        match(test[3], 12, 15, true),
        match(test[3], 16, 19, true)}} });
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
        match(test[0], 0, 4, true)},
    {
        match(test[1], 0, 5, true)},
    { },
    { },
    {
        match(test[4], 0, 13, true)},
    { },
    {
        match(test[6], 0, 10, true)},
    {
        match(test[7], 0, 25, true)},
    {
        match(test[8], 0, 11, true)},
    {
        match(test[9], 5, 9, true),
        match(test[9], 20, 27, true),
        match(test[9], 29, 36, true),
        match(test[9], 41, 48, true)}} });
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
        match(test[2], 0, 3, true)},
    {
        match(test[3], 0, 4, true)},
    { },
    {
        match(test[5], 0, 5, true)},
    {
        match(test[6], 0, 7, true)},
    {
        match(test[7], 7, 10, true),
        match(test[7], 14, 20, true),
        match(test[7], 31, 34, true)}} });
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
        match(test[5], 0, 3, true)},
    {
        match(test[6], 0, 4, true)},
    {
        match(test[7], 0, 5, true)},
    {
        match(test[8], 0, 8, true)},
    {
        match(test[9], 3, 13, true),
        match(test[9], 14, 22, true),
        match(test[9], 22, 29, true),
        match(test[9], 37, 50, true)}} });
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
        match(test[3], 0, 5, true)},
    {
        match(test[4], 0, 6, true)},
    {
        match(test[5], 0, 7, true)},
    {
        match(test[6], 1, 9, true)},
    { }, { }, { }, { }, { }, { }, { },
    {
        match(test[14], 1, 6, true),
        match(test[14], 7, 13, true),
        match(test[14], 20, 28, true),
        match(test[14], 31, 37, true),
        match(test[14], 47, 54, true)}} });
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
        match(test[2], 0, 6, true)},
    {
        match(test[3], 0, 7, true)},
    {
        match(test[4], 0, 7, true)},
    {
        match(test[5], 0, 7, true)}} });
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
        match(test[1], 0, 3, true)},
    {
        match(test[2], 0, 4, true)},
    {
        match(test[3], 0, 5, true)},
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
        match(test[1], 0, 1, true)},
    {
        match(test[2], 0, 2, true)},
    {
        match(test[3], 0, 3, true)},
    {
        match(test[4], 2, 6, true)},
    {
        match(test[5], 0, 5, true)},
    {
        match(test[6], 4, 10, true)},
    {
        match(test[7], 0, 7, true)},
    {
        match(test[8], 4, 10, true)}} });;
}


} //end of namespace assignment1