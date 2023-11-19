#ifndef __DATA_READER_H__
#define __DATA_READER_H__
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

namespace assignment1 {
using namespace std;

template <typename Data>
class MyFileReader {
public:
    MyFileReader() : 
        m_tokens(), m_cur_line(0), 
        m_is_file_open(false), m_unexcepted_file_end(false) {}

    //���� �б� ���� �� true, �ƴ� �� false ��ȯ
    virtual bool read(Data& container) final { 
        if (!m_is_file_open) {
            cout << "[error] �׽�Ʈ ������ ������ ������ �ʾҽ��ϴ�." << endl;
            return false;
        }

        return read_file(container); 
    }
    virtual bool open(const char* filename) final {
        //���� ����
        m_filename = filename;
        if (opened_file.is_open()) opened_file.close();

        opened_file.open(filename);
        if (!opened_file.is_open())
            return (m_is_file_open = false);

        return (m_is_file_open = true);
    }


private:
    string m_filename;
    ifstream opened_file;
    bool m_is_file_open;

    bool read_file(Data& container) {
        //�� �پ� �б�
        m_cur_line = 0;
        bool read_successed = true;
        string line;

        while (getline(opened_file, line)) {
            int cur_line_bak = ++m_cur_line;

            //�� �� ��ūȭ
            m_tokens.clear();
            string token;
            stringstream sstream(line);
            while (sstream >> token) {
                if (is_comment(token)) break;
                m_tokens.push_back(token);
            }
            if (m_tokens.empty()) continue;

            //�� �ٸ��� �۾� ����
            read_successed = single_line_action(container);
            if (!read_successed) {
                m_unexcepted_file_end = true;
                break;
            }

            m_cur_line = cur_line_bak;
        }

        //���� ������ �ʿ��� �۾� ����
        m_tokens.clear();
        read_successed &= end_file_action(container);
        opened_file.close();
        return read_successed;
    }

    bool is_comment(const string& str) {
        if (str.size() >= 1 && str[0] == '#')
            return true;
        else if (str.size() >= 2)
            return str.substr(0, 2) == "//";
        else return false;
    }


protected:
    const string& get_filename() { return m_filename; }
    vector<string> m_tokens;
    int m_cur_line;
    int m_unexcepted_file_end;

    //�Ļ� Ŭ�������� Ư��ȭ�� �۾� ����
    //false: �۾� ����  true: �۾� ����
    virtual bool single_line_action(Data& container) = 0;
    virtual bool end_file_action(Data& container) = 0;
};


}
#endif