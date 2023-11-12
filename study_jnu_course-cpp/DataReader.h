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
class DataReader {
public:
    void read(const char* filename, Data& container) { 
        read_file(filename, container); 
    }


private:
    virtual bool read_file(const char* filename, Data& container) final {
        //���� ����
        ifstream opened_file;
        file_read_successed = true;

        opened_file.open(filename);
        if (!opened_file.is_open()) {
            cout << "\"" << filename << "\" ������ ���� �� �����ϴ�." << endl;
            return false;
        }

        //�� �پ� �б�
        cur_line = 0;
        string line;
        while (getline(opened_file, line)) {
            cur_line++;
            if (is_comment_or_blank(line)) continue;

            //�� �� ��ūȭ
            tokens.clear();
            string split;
            stringstream sstream(line);
            while (getline(sstream, split, ' ')) {
                string token = trim(split);
                if (token.size() != 0)
                    tokens.push_back(token);
            }

            //�� �ٸ��� �۾� ����
            file_read_successed = single_line_action(container);
            if (!file_read_successed) break;
        }

        //���� ������ �ʿ��� �۾� ����
        file_read_successed &= end_file_action(container);
        opened_file.close();
        return file_read_successed;
    }

    virtual bool is_comment_or_blank(const string& line) final {
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
    int cur_line;
    vector<string> tokens;
    bool file_read_successed;

    //�Ļ� Ŭ�������� Ư��ȭ�� �۾� ����
    //false: �۾� ����  true: �۾� ����
    virtual bool single_line_action(Data& container) = 0;
    virtual bool end_file_action(Data& container) = 0;
};

}
#endif