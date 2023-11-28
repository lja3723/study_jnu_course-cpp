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

    //파일 오픈 성공 시 true, 실패 시 false
    virtual bool open(const char* filename) final {
        //파일 열기
        m_filename = filename;
        if (opened_file.is_open()) opened_file.close();

        opened_file.open(filename);
        if (!opened_file.is_open())
            return (m_is_file_open = false);

        return (m_is_file_open = true);
    }
    
    //파일 파싱 성공 시 true, 실패 시 false 반환
    virtual bool read(Data& container) final {
        if (!m_is_file_open) {
            cout << "[error] 파일이 열리지 않았습니다." << endl;
            return false;
        }

        return read_file(container);
    }


private:
    string m_filename;
    ifstream opened_file;
    bool m_is_file_open;

    //오픈한 파일 파싱하면서 파싱된 정보를 container에 저장
    bool read_file(Data& container) {
        m_cur_line = 0;
        bool read_successed = true;
        bool multiline = false;
        string line;

        //한 줄씩 읽기
        while (getline(opened_file, line)) {
            int cur_line_bak = ++m_cur_line;

            stringstream sstream(line);
            //멀티라인 모드가 아닌 경우
            if (!multiline) {
                while (sstream >> m_token) {
                    if (is_comment(m_token)) break;

                    // ""로 감싸진 문자열인 경우 파싱
                    m_token = check_quote(line, sstream);
                    if (m_token.empty()) {
                        showline();
                        cout << "큰따옴표 매치가 잘못되었습니다." << endl;
                        opened_file.close();
                        return false;
                    }

                    // 멀티라인인 경우 파싱
                    if (m_token == "'''") {
                        m_token = "";
                        multiline = true;
                        break;
                    }

                    //!멀티라인모드 && !""모드인 경우
                    //토큰을 파싱해서 토큰 행동 실행
                    read_successed = token_action(container);
                    if (!read_successed) {
                        m_unexcepted_file_end = true;
                        break;
                    }
                }
            }

            //멀티라인 모드에서 ''' 를 만난 경우(멀티라인 종료)
            else if (trim(line) == "'''") {
                m_token.pop_back();
                read_successed = token_action(container);
                if (!read_successed) {
                    m_unexcepted_file_end = true;
                }
                multiline = false;
            }

            //멀티라인 모드인 경우 (멀티라인 진행중)
            else {
                m_token += line + string(" ");
            }

            if (m_unexcepted_file_end) break;

            m_cur_line = cur_line_bak;
        }

        //파일 끝에서 필요한 작업 수행
        m_token.clear();
        read_successed &= end_file_action(container);
        opened_file.close();
        return read_successed;
    }

    //주석 체크
    bool is_comment(const string& str) {
        if (str.size() >= 1 && str[0] == '#')
            return true;
        else if (str.size() >= 2)
            return str.substr(0, 2) == "//";
        else return false;
    }

    //큰따옴표 처리된 토큰 여부 체크; 큰따옴표 짝이 맞지 않는 경우 empty 리턴
    string check_quote(string& line, stringstream& ss) {
        //큰따옴표 토큰 아닌경우 그대로 반환
        if (m_token.empty()) return "";
        if (m_token[0] != '\"') return m_token; 

        //현재 라인에서 첫 번쨰 " 찾기
        size_t pos = line.find('\"'); 

        //현재 라인에서 두 번째 " 찾기
        string ret = "";
        for (pos++; pos < line.size(); pos++) {

            //  \"(escaped된 ") 인 경우 건너뜀
            if (line[pos] == '\"' && line[pos - 1] == '\\') {
                ret.back() = '\"';
                continue;
            }

            //두번째 " 찾음
            if (line[pos] == '\"') {

                //현재 토큰이 un-escaped된 "을 가진 경우
                size_t pos = m_token.find("\\\"", 1);
                if (pos == string::npos) {
                    pos = m_token.find('\"', 1);
                    if (pos != string::npos)
                        return ret; //큰따옴표 처리된 토큰 반환
                }
                
                //두번째 "를 가진 토큰을 찾음
                while (ss >> m_token) {
                    size_t pos = 0;
                    while (true) {
                        pos = m_token.find('\"', pos);
                        if (pos != string::npos && pos > 0 && m_token[pos - 1] == '\\') {
                            pos++;
                        }
                        else if (pos != string::npos)
                            return ret; //큰따옴표 처리된 토큰 반환
                        else
                            break;
                    }
                }
            }

            ret += line[pos];
        }

        return "";
    }

    //양쪽 trim 수행
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

    //파생 클래스마다 특수화된 작업 실행
    //false: 작업 실패  true: 작업 성공
    virtual bool token_action(Data& container) = 0;
    virtual bool end_file_action(Data& container) = 0;
};


}
#endif