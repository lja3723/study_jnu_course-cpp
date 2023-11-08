//깃 컨벤션 참고 https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
using namespace std;

namespace assignment1 {


class regex {
private:
    //매치 객체 인터페이스
    class Imatchable {
        public: virtual bool test(char _match) = 0;
    };

    //매치 객체 구현(싱글 문자)
    class matcher_single : public Imatchable {
        char ch;
    public:
        matcher_single(char ch) : ch(ch) {}
        virtual bool test(char _match) {
            return ch == _match;
        }
    };

    //매치 객체 구현(A|B)
    class matcher_or : public Imatchable {
        char a, b;
    public:
        matcher_or(char a, char b) : a(a), b(b) {}
        virtual bool test(char _match) {
            return a == _match || b == _match;
        }
    };

    //매치 객체 구현(.)
    class matcher_dot : public Imatchable {
    public:
        virtual bool test(char _match) {
            return ('A' <= _match && _match <= 'Z') || ('a' <= _match && _match <= 'z');
        }
    };

    //매치 객체 구현({m, n})
    //TODO
    class matcher_counter : public Imatchable {
        int counter;
        int m, n;
    public:
        matcher_counter(int m, int n) : m(m), n(n) {
            counter = 0;
        }
        virtual bool test(char _match) {
            //TODO
            return false;
        }
    };

    //TODO
    class matcher_generator {
    private:
        static vector<Imatchable*> list; //생성된 매쳐들을 담는 컨테이너
        matcher_generator() { }

    public:
        static Imatchable* gen(const string& str) {
            //TODO
        }
    };

    class node {
    private:
        string name; // state node name
        bool transited; // for syncronization
        bool state; // 0: deactivated, 1: activated
        bool isTerminal; // 0: terminal, 1: non-terminal
        Imatchable* match; // target character for matching
        vector<node*> next; // next link 

    public:
        node() {
            state = transited = isTerminal = 0;
            match = nullptr;
        }
        void init(const string& _name, bool _isTerminal, Imatchable* _match) {
            name = _name;
            isTerminal = _isTerminal;
            match = _match;
            state = transited = 0;
        }
        void addNode(node* _next) {
            next.push_back(_next);
        }
        // for syncronization
        node& transition() {
            transited = 1;
            return *this;
        }
        void activation() {
            state = transited;
            transited = 0;
        }
        //터미널 노드면서 인식에 성공하면 true 반환
        bool input(const char& ch) {
            bool ret = false; 
            //match == ch 역할을 하는 새 객체 필요
            //상속으로 구현해보자
            if (state && match->test(ch)) {
                if (isTerminal) {
                    ret = true;
                    cout << "  >>>> accepted by " << name << endl;
                }

                for (int i = 0; i < next.size(); i++) {
                    next[i]->transition();
                }
            }
            state = 0; // deactivate this node after matching
            return ret;
        }
    };

    vector<node> vn;
    string exp;

public:
    regex(string exp) : vn(NULL), exp(exp) {}
    void set_expr(string exp) {
        this->exp = exp;
    }

    //주어진 정규표현식으로 상태머신 생성
    //잘못된 문법이면 false 반환
    //TODO
    bool compile() {
        //TODO
        //아래는 mock code
        //문법을 해석해 아래 작업이 알아서 되어야함
        vn.resize(5/*정규표현식 해석 후 노드 개수가 되어야 함*/);

        // set nodes (name, termianl, match)
        vn[0].init("s0", 0, new matcher_single('a'));
        vn[1].init("s1", 0, new matcher_single('b'));
        vn[2].init("s2", 1, new matcher_single('c'));
        vn[3].init("s3", 0, new matcher_single('d'));
        vn[4].init("s4", 1, new matcher_single('e'));

        // set links
        vn[0].addNode(&vn[1]);
        vn[0].addNode(&vn[3]);
        vn[1].addNode(&vn[2]);
        vn[3].addNode(&vn[4]);

        return false;
    }
    //컴파일한 후 실행되어야 됨
    //인식하면 true, 그렇지 않으면 false 반환
    bool test(const string& str) {
        bool ret = false;
        cout << "test for ' " << str << "'" << endl;

        for (int i = 0; i < str.length(); i++) {
            cout << "  >> input " << str[i] << endl;

            // epsilon activation for s0
            vn[0].transition().activation();

            // give ch all nodes
            for (int j = 0; j < vn.size(); j++)
                if (vn[j].input(str[i]))
                    ret = true;

            // determine transited state
            for (int j = 0; j < vn.size(); j++)
                vn[j].activation();
        }
        cout << endl;
        return ret;
    }


};

int main() {
    regex re("abcde");
     re.compile();

    re.test("abc");
    re.test("ade");
    re.test("aaaaabbbbabcaaaaaadeaaaaabc");

    return 0;
}

}

//int main() { assignment1::main(); }
