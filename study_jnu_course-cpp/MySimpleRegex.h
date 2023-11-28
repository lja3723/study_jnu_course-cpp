#ifndef __MY_SIMPLE_REGEX_H__
#define __MY_SIMPLE_REGEX_H__
#include <iostream>
#include <vector>
#include <map>

namespace assignment1 {
using namespace std;


//특정한 구간을 표현하는 문자열
struct ranged_string {
    const string ref;           //참조 문자열
    const bool is_valid;        //객체 유효성
    const size_t start, end;  //참조 문자열에서의 범위

    ranged_string(const string& ref, const size_t start, const size_t end, const bool is_valid) :
        ref(ref), start(start), end(end), is_valid(is_valid) {}

    //범위
    pair<size_t, size_t> span() const { return pair<size_t, size_t>(start, end); }

    //범위에 속하는 문자열 반환
    string group() const { return ref.substr(start, size_t(end - start)); }

    //유효하지 않은 ranged_string 객체를 반환한다.
    static const ranged_string invalid() { return ranged_string(string(), 0, 0, false); }

    //cout 출력 연산자 오버로딩
    friend ostream& operator<<(ostream&, const ranged_string&);
};



//Regex를 입력으로 받아 패턴을 인식하는 래퍼 클래스
//MySimpleRegex::compiled 클래스가 본체이다.
class MySimpleRegex {
private: MySimpleRegex();
    //이 클래스는 static 메서드만 있으므로 생성자를 private로 둔다.

public:
    class compiled; //진짜 기능을 하는 클래스
    
    //정규표현식을 컴파일한 객체 compiled를 반환한다.
    //정규식이 바르지 않은 경우 is_valid가 false가 된다.
    static compiled compile(const string& m_regex);

    //source에서 정규표현식과 가장 먼저 일치하는 범위를 구한다.
    //그런 범위가 없으면 invalid한 객체를 반환한다.
    static ranged_string match(const string& m_regex, const string& source, size_t index_start = 0);

    //source에서 정규표현식과 일치하는 모든 범위를 구한다
    //일치정보가 없을 경우 empty()한 vector가 반환된다.
    static vector<ranged_string> match_all(const string m_regex, const string& source);
};



/*
class MySimpleRegex::compiled:
정규표현식을 컴파일한 객체이다. 생성자 호출 시 정규표현식을 입력받는다.
입력으로 들어온 정규표현식을 파싱 후 그에 맞는 state-machine을 내부적으로 생성한다.

범위를 구할 input 문자열이 들어오면 상태기계를 이용해 매치되는 범위를 구한다.
일치하는 범위가 없으면 invalid한 ranged_string이 반환된다.

다양한 문자열을 비교하기 위한 매치 클래스 및 파생 클래스와
다양한 노드 포인터를 포현하기 위한 노드 포인터 클래스 및 파생 클래스를 정의한다.

*/
class MySimpleRegex::compiled {
public:
    //주어진 정규표현식을 파싱해 내부적으로 상태기계를 생성한다.
    compiled(const string& m_regex);

    //생성된 상태기계를 삭제한다. 메모리 반환 작업을 수행한다.
    virtual ~compiled();

    //입력된 정규표현식이 문법적으로 바른지 확인한다.
    bool is_valid();

    //source에서 start_idx부터 탐색을 시작해 가장 처음으로 발견된 일치 정보를 반환한다.
    ranged_string match(const string& source, size_t index_start = 0);

    //source에서 발견되는 모든 일치 정보를 반환한다.
    //아무 구간도 매치되지 않으면 empty() 한 vector가 반환된다.
    vector<ranged_string> match_all(const string& source);



private:
    /****************************/
    /*      Inner Classes       */
    /****************************/

    //state machine 생성기 클래스
    //regex를 입력하면 그것과 일치하는 상태 머신을 생성한다.
    //정의: MySimpleRegex.h
    class state_machine_creator;


    //input char을 판별하기 위한 매치 클래스
    //상속으로 다형성을 구현한다.
    //정의: MySimpleRegex.h
    class Imatchable;       //매치 객체 인터페이스    
    class matcher_single;   //단일 문자 매치    
    class matcher_dot;      //모든 문자 매치(개행 문자 제외)
    class matcher_true;     //무조건 매치


    //상태머신의 노드와 노드 포인터를 표현하는 클래스이다    
    //node를 가리키는 node_ptr 추상 클래스와 파생 클래스로 구성된다.
    //활성화 신호를 바로 전달하는 node_ptr_direct 포인터,
    //내부에 카운터가 있어 카운트가 일정 이상이 되면 활성화 신호를 전달하는
    //node_ptr_cnt_inner 포인터가 존재한다.
    //정의: MySimpleRegex_node.h
    class node;         //상태머신의 노드
    class node_ptr;         
    class node_ptr_direct;  
    class node_ptr_inner_counter;


    //다음 활성화 노드에 대한 정보로, 다음 활성화 노드 대상인 target과, 
    //활성화 상태와 같이 전이되는 상태인 start_index 정보를 담는다.
    struct active_request_info {
        node* target;
        size_t start_index;
    };


    /****************************/
    /*      Private fields      */
    /****************************/
    vector<node*> m_node;   //상태기계 노드 컨테이너 (원본 포인터)
    node* m_epsilon;        //엡실론 신호를 next에 주는 노드
    node* m_terminal;       //터미널 노드 리스트 (얕은 복사됨)
    const string m_regex;   //저장된 정규표현식
    bool m_is_valid;        //정규표현식이 바른 지 여부



    /****************************/
    /*    Private functions     */
    /****************************/ 
    ranged_string state_machine_input(      //상태기계에 문자열을 입력으로 넣어주면
        const string& src,                  //가장 처음으로 accept된 일치 정보를 출력한다.
        size_t index_start = 0,           //매치된 구간이 없으면 invalid한 객체를 반환한다.
        bool check_at_front_only = false);

    //엡실론 신호를 노드에 부여한다.
    void give_epsilon(
        vector<active_request_info>& next_actives, //다음 활성화될 노드 리스트
        map<string, node*>& actives,    //활성화된 노드 리스트
        size_t state_istart,          //해당 노드의 첫 활성화 신호 시 인덱스값
        bool check_at_front_only);      //첫 부분부터 정확히 일치 여부 확인

    //다음 활성화될 노드 리스트(next_actives)를 참고하여 활성화될 노드들을 실제로 활성화한다.
    void active_transition(
        vector<active_request_info>& next_actives,  
        map<string, node*>& actives);
    
    //터미널 노드 순회 후 accepted되었으면 found에 matched를 생성한다.
    void check_terminal_active(
        map<size_t, ranged_string*>& found, 
        const string& src, 
        size_t idx);
};



/***********   문자 matcher 및 파생 클래스   ***********/
using compiled = MySimpleRegex::compiled;
class compiled::Imatchable {
public:
    virtual bool test(char ch) = 0;
    virtual Imatchable* copy() = 0; //자기 자신 동적 복사


};
class compiled::matcher_single : public compiled::Imatchable {
private: char ch;
public:
    matcher_single(char ch) : ch(ch) {}
    virtual bool test(char _ch) override { return ch == _ch; }
    virtual matcher_single* copy() override {
        return new matcher_single(ch);
    }


};
class compiled::matcher_dot : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return ('A' <= _ch && _ch <= 'Z') || ('a' <= _ch && _ch <= 'z');
    }

    virtual matcher_dot* copy() override {
        return new matcher_dot();
    }


};
class compiled::matcher_true : public compiled::Imatchable {
public:
    virtual bool test(char _ch) override {
        return true;
    }

    virtual matcher_true* copy() override {
        return new matcher_true();
    }


};


} //end of namespace assignment1
#endif