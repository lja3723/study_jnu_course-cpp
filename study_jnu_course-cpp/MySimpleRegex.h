#ifndef __MY_SIMPLE_REGEX_H__
#define __MY_SIMPLE_REGEX_H__
#include <iostream>
#include <vector>
#include <map>

namespace assignment1 {
using namespace std;


//Ư���� ������ ǥ���ϴ� ���ڿ�
struct ranged_string {
    const string ref;           //���� ���ڿ�
    const bool is_valid;        //��ü ��ȿ��
    const size_t start, end;  //���� ���ڿ������� ����

    ranged_string(const string& ref, const size_t start, const size_t end, const bool is_valid) :
        ref(ref), start(start), end(end), is_valid(is_valid) {}

    //����
    pair<size_t, size_t> span() const { return pair<size_t, size_t>(start, end); }

    //������ ���ϴ� ���ڿ� ��ȯ
    string group() const { return ref.substr(start, size_t(end - start)); }

    //��ȿ���� ���� ranged_string ��ü�� ��ȯ�Ѵ�.
    static const ranged_string invalid() { return ranged_string(string(), 0, 0, false); }

    //cout ��� ������ �����ε�
    friend ostream& operator<<(ostream&, const ranged_string&);
};



//Regex�� �Է����� �޾� ������ �ν��ϴ� ���� Ŭ����
//MySimpleRegex::compiled Ŭ������ ��ü�̴�.
class MySimpleRegex {
private: MySimpleRegex();
    //�� Ŭ������ static �޼��常 �����Ƿ� �����ڸ� private�� �д�.

public:
    class compiled; //��¥ ����� �ϴ� Ŭ����
    
    //����ǥ������ �������� ��ü compiled�� ��ȯ�Ѵ�.
    //���Խ��� �ٸ��� ���� ��� is_valid�� false�� �ȴ�.
    static compiled compile(const string& m_regex);

    //source���� ����ǥ���İ� ���� ���� ��ġ�ϴ� ������ ���Ѵ�.
    //�׷� ������ ������ invalid�� ��ü�� ��ȯ�Ѵ�.
    static ranged_string match(const string& m_regex, const string& source, size_t index_start = 0);

    //source���� ����ǥ���İ� ��ġ�ϴ� ��� ������ ���Ѵ�
    //��ġ������ ���� ��� empty()�� vector�� ��ȯ�ȴ�.
    static vector<ranged_string> match_all(const string m_regex, const string& source);
};



/*
class MySimpleRegex::compiled:
����ǥ������ �������� ��ü�̴�. ������ ȣ�� �� ����ǥ������ �Է¹޴´�.
�Է����� ���� ����ǥ������ �Ľ� �� �׿� �´� state-machine�� ���������� �����Ѵ�.

������ ���� input ���ڿ��� ������ ���±�踦 �̿��� ��ġ�Ǵ� ������ ���Ѵ�.
��ġ�ϴ� ������ ������ invalid�� ranged_string�� ��ȯ�ȴ�.

�پ��� ���ڿ��� ���ϱ� ���� ��ġ Ŭ���� �� �Ļ� Ŭ������
�پ��� ��� �����͸� �����ϱ� ���� ��� ������ Ŭ���� �� �Ļ� Ŭ������ �����Ѵ�.

*/
class MySimpleRegex::compiled {
public:
    //�־��� ����ǥ������ �Ľ��� ���������� ���±�踦 �����Ѵ�.
    compiled(const string& m_regex);

    //������ ���±�踦 �����Ѵ�. �޸� ��ȯ �۾��� �����Ѵ�.
    virtual ~compiled();

    //�Էµ� ����ǥ������ ���������� �ٸ��� Ȯ���Ѵ�.
    bool is_valid();

    //source���� start_idx���� Ž���� ������ ���� ó������ �߰ߵ� ��ġ ������ ��ȯ�Ѵ�.
    ranged_string match(const string& source, size_t index_start = 0);

    //source���� �߰ߵǴ� ��� ��ġ ������ ��ȯ�Ѵ�.
    //�ƹ� ������ ��ġ���� ������ empty() �� vector�� ��ȯ�ȴ�.
    vector<ranged_string> match_all(const string& source);



private:
    /****************************/
    /*      Inner Classes       */
    /****************************/

    //state machine ������ Ŭ����
    //regex�� �Է��ϸ� �װͰ� ��ġ�ϴ� ���� �ӽ��� �����Ѵ�.
    //����: MySimpleRegex.h
    class state_machine_creator;


    //input char�� �Ǻ��ϱ� ���� ��ġ Ŭ����
    //������� �������� �����Ѵ�.
    //����: MySimpleRegex.h
    class Imatchable;       //��ġ ��ü �������̽�    
    class matcher_single;   //���� ���� ��ġ    
    class matcher_dot;      //��� ���� ��ġ(���� ���� ����)
    class matcher_true;     //������ ��ġ


    //���¸ӽ��� ���� ��� �����͸� ǥ���ϴ� Ŭ�����̴�    
    //node�� ����Ű�� node_ptr �߻� Ŭ������ �Ļ� Ŭ������ �����ȴ�.
    //Ȱ��ȭ ��ȣ�� �ٷ� �����ϴ� node_ptr_direct ������,
    //���ο� ī���Ͱ� �־� ī��Ʈ�� ���� �̻��� �Ǹ� Ȱ��ȭ ��ȣ�� �����ϴ�
    //node_ptr_cnt_inner �����Ͱ� �����Ѵ�.
    //����: MySimpleRegex_node.h
    class node;         //���¸ӽ��� ���
    class node_ptr;         
    class node_ptr_direct;  
    class node_ptr_inner_counter;


    //���� Ȱ��ȭ ��忡 ���� ������, ���� Ȱ��ȭ ��� ����� target��, 
    //Ȱ��ȭ ���¿� ���� ���̵Ǵ� ������ start_index ������ ��´�.
    struct active_request_info {
        node* target;
        size_t start_index;
    };


    /****************************/
    /*      Private fields      */
    /****************************/
    vector<node*> m_node;   //���±�� ��� �����̳� (���� ������)
    node* m_epsilon;        //���Ƿ� ��ȣ�� next�� �ִ� ���
    node* m_terminal;       //�͹̳� ��� ����Ʈ (���� �����)
    const string m_regex;   //����� ����ǥ����
    bool m_is_valid;        //����ǥ������ �ٸ� �� ����



    /****************************/
    /*    Private functions     */
    /****************************/ 
    ranged_string state_machine_input(      //���±�迡 ���ڿ��� �Է����� �־��ָ�
        const string& src,                  //���� ó������ accept�� ��ġ ������ ����Ѵ�.
        size_t index_start = 0,           //��ġ�� ������ ������ invalid�� ��ü�� ��ȯ�Ѵ�.
        bool check_at_front_only = false);

    //���Ƿ� ��ȣ�� ��忡 �ο��Ѵ�.
    void give_epsilon(
        vector<active_request_info>& next_actives, //���� Ȱ��ȭ�� ��� ����Ʈ
        map<string, node*>& actives,    //Ȱ��ȭ�� ��� ����Ʈ
        size_t state_istart,          //�ش� ����� ù Ȱ��ȭ ��ȣ �� �ε�����
        bool check_at_front_only);      //ù �κк��� ��Ȯ�� ��ġ ���� Ȯ��

    //���� Ȱ��ȭ�� ��� ����Ʈ(next_actives)�� �����Ͽ� Ȱ��ȭ�� ������ ������ Ȱ��ȭ�Ѵ�.
    void active_transition(
        vector<active_request_info>& next_actives,  
        map<string, node*>& actives);
    
    //�͹̳� ��� ��ȸ �� accepted�Ǿ����� found�� matched�� �����Ѵ�.
    void check_terminal_active(
        map<size_t, ranged_string*>& found, 
        const string& src, 
        size_t idx);
};



/***********   ���� matcher �� �Ļ� Ŭ����   ***********/
using compiled = MySimpleRegex::compiled;
class compiled::Imatchable {
public:
    virtual bool test(char ch) = 0;
    virtual Imatchable* copy() = 0; //�ڱ� �ڽ� ���� ����


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