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
    //���� �б� ���� �� true, �ƴ� �� false ��ȯ
    virtual bool read(const char* filename, Data& container) final { 
        return read_file(filename, container); 
    }


private:
    bool read_file(const char* filename, Data& container) {
        //���� ����
        ifstream opened_file;
        bool read_successed = true;

        opened_file.open(filename);
        if (!opened_file.is_open()) {
            cout << "\"" << filename << "\" ������ ���� �� �����ϴ�." << endl;
            return false;
        }

        //�� �پ� �б�
        int cur_line = 0;
        string line;
        while (getline(opened_file, line)) {
            cur_line++;
            if (is_comment_or_blank(line)) continue;

            //�� �� ��ūȭ
            vector<string> tokens;
            string split;
            stringstream sstream(line);
            while (getline(sstream, split, ' ')) {
                string token = trim(split);
                if (token.size() != 0)
                    tokens.push_back(token);
            }

            //�� �ٸ��� �۾� ����
            read_successed = single_line_action(container, tokens, cur_line);
            if (!read_successed) break;
        }

        //���� ������ �ʿ��� �۾� ����
        read_successed &= end_file_action(container, read_successed);
        opened_file.close();
        return read_successed;
    }

    bool is_comment_or_blank(const string& line) {
        string l = trim(line);
        if (l.size() >= 2)  return l.substr(0, 2) == "//";
        else if (l.empty()) return true;
        else                return false;
    }

    string trim(const string& str) {
        std::string s(str);
        s.erase(0, s.find_first_not_of(" \n\r\t\f\v"));
        s.erase(s.find_last_not_of(" \n\r\t\f\v") + 1);
        return s;
    }


protected:

    //�Ļ� Ŭ�������� Ư��ȭ�� �۾� ����
    //false: �۾� ����  true: �۾� ����
    virtual bool single_line_action(
        Data& container, vector<string>& tokens, int cur_line) = 0;

    virtual bool end_file_action(Data& container, bool read_successed) = 0;
};

}
#endif