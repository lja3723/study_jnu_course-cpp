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

    //파일 읽기 성공 시 true, 아닐 시 false 반환
    virtual bool read(Data& container) final { 
        if (!m_is_file_open) {
            cout << "[error] 테스트 데이터 파일이 열리지 않았습니다." << endl;
            return false;
        }

        return read_file(container); 
    }
    virtual bool open(const char* filename) final {
        //파일 열기
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
        //한 줄씩 읽기
        m_cur_line = 0;
        bool read_successed = true;
        string line;

        while (getline(opened_file, line)) {
            int cur_line_bak = ++m_cur_line;

            //한 줄 토큰화
            m_tokens.clear();
            string token;
            stringstream sstream(line);
            while (sstream >> token) {
                if (is_comment(token)) break;
                m_tokens.push_back(token);
            }
            if (m_tokens.empty()) continue;

            //각 줄마다 작업 수행
            read_successed = single_line_action(container);
            if (!read_successed) {
                m_unexcepted_file_end = true;
                break;
            }

            m_cur_line = cur_line_bak;
        }

        //파일 끝에서 필요한 작업 수행
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

    //파생 클래스마다 특수화된 작업 실행
    //false: 작업 실패  true: 작업 성공
    virtual bool single_line_action(Data& container) = 0;
    virtual bool end_file_action(Data& container) = 0;
};


}
#endif