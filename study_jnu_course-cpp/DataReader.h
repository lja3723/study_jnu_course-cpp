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
        //파일 열기
        ifstream opened_file;
        file_read_successed = true;

        opened_file.open(filename);
        if (!opened_file.is_open()) {
            cout << "\"" << filename << "\" 파일을 읽을 수 없습니다." << endl;
            return false;
        }

        //한 줄씩 읽기
        cur_line = 0;
        string line;
        while (getline(opened_file, line)) {
            cur_line++;
            if (is_comment_or_blank(line)) continue;

            //한 줄 토큰화
            tokens.clear();
            string split;
            stringstream sstream(line);
            while (getline(sstream, split, ' ')) {
                string token = trim(split);
                if (token.size() != 0)
                    tokens.push_back(token);
            }

            //각 줄마다 작업 수행
            file_read_successed = single_line_action(container);
            if (!file_read_successed) break;
        }

        //파일 끝에서 필요한 작업 수행
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

    //파생 클래스마다 특수화된 작업 실행
    //false: 작업 실패  true: 작업 성공
    virtual bool single_line_action(Data& container) = 0;
    virtual bool end_file_action(Data& container) = 0;
};

}
#endif