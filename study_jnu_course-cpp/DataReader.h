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
    //파일 읽기 성공 시 true, 아닐 시 false 반환
    virtual bool read(const char* filename, Data& container) final { 
        return read_file(filename, container); 
    }


private:
    bool read_file(const char* filename, Data& container) {
        //파일 열기
        ifstream opened_file;
        bool read_successed = true;

        opened_file.open(filename);
        if (!opened_file.is_open()) {
            cout << "\"" << filename << "\" 파일을 읽을 수 없습니다." << endl;
            return false;
        }

        //한 줄씩 읽기
        int cur_line = 0;
        string line;
        while (getline(opened_file, line)) {
            cur_line++;
            if (is_comment_or_blank(line)) continue;

            //한 줄 토큰화
            vector<string> tokens;
            string split;
            stringstream sstream(line);
            while (getline(sstream, split, ' ')) {
                string token = trim(split);
                if (token.size() != 0)
                    tokens.push_back(token);
            }

            //각 줄마다 작업 수행
            read_successed = single_line_action(container, tokens, cur_line);
            if (!read_successed) break;
        }

        //파일 끝에서 필요한 작업 수행
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

    //파생 클래스마다 특수화된 작업 실행
    //false: 작업 실패  true: 작업 성공
    virtual bool single_line_action(
        Data& container, vector<string>& tokens, int cur_line) = 0;

    virtual bool end_file_action(Data& container, bool read_successed) = 0;
};

}
#endif