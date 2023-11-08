//�� ������ ���� https://github.com/JNU-econovation/Let-s-git-it-started/blob/main/Glossary_of_Terms/Commit%20Convention.md

#include <iostream>
#include <vector>
using namespace std;

namespace assignment1 {


class regex {
private:
    //��ġ ��ü �������̽�
    class Imatchable {
        public: virtual bool test(char _match) = 0;
    };

    //��ġ ��ü ����(�̱� ����)
    class matcher_single : public Imatchable {
        char ch;
    public:
        matcher_single(char ch) : ch(ch) {}
        virtual bool test(char _match) {
            return ch == _match;
        }
    };

    //��ġ ��ü ����(A|B)
    class matcher_or : public Imatchable {
        char a, b;
    public:
        matcher_or(char a, char b) : a(a), b(b) {}
        virtual bool test(char _match) {
            return a == _match || b == _match;
        }
    };

    //��ġ ��ü ����(.)
    class matcher_dot : public Imatchable {
    public:
        virtual bool test(char _match) {
            return ('A' <= _match && _match <= 'Z') || ('a' <= _match && _match <= 'z');
        }
    };

    //��ġ ��ü ����({m, n})
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
        static vector<Imatchable*> list; //������ ���ĵ��� ��� �����̳�
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
        //�͹̳� ���鼭 �νĿ� �����ϸ� true ��ȯ
        bool input(const char& ch) {
            bool ret = false; 
            //match == ch ������ �ϴ� �� ��ü �ʿ�
            //������� �����غ���
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

    //�־��� ����ǥ�������� ���¸ӽ� ����
    //�߸��� �����̸� false ��ȯ
    //TODO
    bool compile() {
        //TODO
        //�Ʒ��� mock code
        //������ �ؼ��� �Ʒ� �۾��� �˾Ƽ� �Ǿ����
        vn.resize(5/*����ǥ���� �ؼ� �� ��� ������ �Ǿ�� ��*/);

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
    //�������� �� ����Ǿ�� ��
    //�ν��ϸ� true, �׷��� ������ false ��ȯ
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
