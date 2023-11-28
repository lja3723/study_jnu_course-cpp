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
        m_token(), m_cur_line(0), 
        m_is_file_open(false), m_unexcepted_file_end(false) {}

    //���� ���� ���� �� true, ���� �� false
    virtual bool open(const char* filename) final {
        //���� ����
        m_filename = filename;
        if (opened_file.is_open()) opened_file.close();

        opened_file.open(filename);
        if (!opened_file.is_open())
            return (m_is_file_open = false);

        return (m_is_file_open = true);
    }
    
    //���� �Ľ� ���� �� true, ���� �� false ��ȯ
    virtual bool read(Data& container) final {
        if (!m_is_file_open) {
            cout << "[error] ������ ������ �ʾҽ��ϴ�." << endl;
            return false;
        }

        return read_file(container);
    }


private:
    string m_filename;
    ifstream opened_file;
    bool m_is_file_open;

    //������ ���� �Ľ��ϸ鼭 �Ľ̵� ������ container�� ����
    bool read_file(Data& container) {
        m_cur_line = 0;
        bool read_successed = true;
        bool multiline = false;
        string line;

        //�� �پ� �б�
        while (getline(opened_file, line)) {
            int cur_line_bak = ++m_cur_line;

            stringstream sstream(line);
            //��Ƽ���� ��尡 �ƴ� ���
            if (!multiline) {
                while (sstream >> m_token) {
                    if (is_comment(m_token)) break;

                    // ""�� ������ ���ڿ��� ��� �Ľ�
                    m_token = check_quote(line, sstream);
                    if (m_token.empty()) {
                        showline();
                        cout << "ū����ǥ ��ġ�� �߸��Ǿ����ϴ�." << endl;
                        opened_file.close();
                        return false;
                    }

                    // ��Ƽ������ ��� �Ľ�
                    if (m_token == "'''") {
                        m_token = "";
                        multiline = true;
                        break;
                    }

                    //!��Ƽ���θ�� && !""����� ���
                    //��ū�� �Ľ��ؼ� ��ū �ൿ ����
                    read_successed = token_action(container);
                    if (!read_successed) {
                        m_unexcepted_file_end = true;
                        break;
                    }
                }
            }

            //��Ƽ���� ��忡�� ''' �� ���� ���(��Ƽ���� ����)
            else if (trim(line) == "'''") {
                m_token.pop_back();
                read_successed = token_action(container);
                if (!read_successed) {
                    m_unexcepted_file_end = true;
                }
                multiline = false;
            }

            //��Ƽ���� ����� ��� (��Ƽ���� ������)
            else {
                m_token += line + string(" ");
            }

            if (m_unexcepted_file_end) break;

            m_cur_line = cur_line_bak;
        }

        //���� ������ �ʿ��� �۾� ����
        m_token.clear();
        read_successed &= end_file_action(container);
        opened_file.close();
        return read_successed;
    }

    //�ּ� üũ
    bool is_comment(const string& str) {
        if (str.size() >= 1 && str[0] == '#')
            return true;
        else if (str.size() >= 2)
            return str.substr(0, 2) == "//";
        else return false;
    }

    //ū����ǥ ó���� ��ū ���� üũ; ū����ǥ ¦�� ���� �ʴ� ��� empty ����
    string check_quote(string& line, stringstream& ss) {
        //ū����ǥ ��ū �ƴѰ�� �״�� ��ȯ
        if (m_token.empty()) return "";
        if (m_token[0] != '\"') return m_token; 

        //���� ���ο��� ù ���� " ã��
        size_t pos = line.find('\"'); 

        //���� ���ο��� �� ��° " ã��
        string ret = "";
        for (pos++; pos < line.size(); pos++) {

            //  \"(escaped�� ") �� ��� �ǳʶ�
            if (line[pos] == '\"' && line[pos - 1] == '\\') {
                ret.back() = '\"';
                continue;
            }

            //�ι�° " ã��
            if (line[pos] == '\"') {

                //���� ��ū�� un-escaped�� "�� ���� ���
                size_t pos = m_token.find("\\\"", 1);
                if (pos == string::npos) {
                    pos = m_token.find('\"', 1);
                    if (pos != string::npos)
                        return ret; //ū����ǥ ó���� ��ū ��ȯ
                }
                
                //�ι�° "�� ���� ��ū�� ã��
                while (ss >> m_token) {
                    size_t pos = 0;
                    while (true) {
                        pos = m_token.find('\"', pos);
                        if (pos != string::npos && pos > 0 && m_token[pos - 1] == '\\') {
                            pos++;
                        }
                        else if (pos != string::npos)
                            return ret; //ū����ǥ ó���� ��ū ��ȯ
                        else
                            break;
                    }
                }
            }

            ret += line[pos];
        }

        return "";
    }

    //���� trim ����
    string& trim(string& str) {
        str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
        str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
        return str;
    }



protected:
    string m_token;
    int m_cur_line;
    int m_unexcepted_file_end;

    void showline() {
        cout << "[error] line " << m_cur_line << " of \"" << m_filename << "\": ";
    }

    //�Ļ� Ŭ�������� Ư��ȭ�� �۾� ����
    //false: �۾� ����  true: �۾� ����
    virtual bool token_action(Data& container) = 0;
    virtual bool end_file_action(Data& container) = 0;
};


}
#endif