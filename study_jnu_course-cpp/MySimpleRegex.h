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
    const unsigned start, end;  //���� ���ڿ������� ����

    ranged_string(const string& ref, const unsigned start, const unsigned end, const bool is_valid) :
        ref(ref), start(start), end(end), is_valid(is_valid) {}

    //����
    pair<unsigned, unsigned> span() const { return pair<unsigned, unsigned>(start, end); }

    //������ ���ϴ� ���ڿ� ��ȯ
    string group() const { return ref.substr(start, size_t(end - start)); }

    //��ȿ���� ���� ranged_string ��ü�� ��ȯ�Ѵ�.
    static const ranged_string invalid() { return ranged_string(string(), 0, 0, false); }
};



//Regex�� �Է����� �޾� ������ �ν��ϴ� ���� Ŭ����
//MySimpleRegex::compiled Ŭ������ ��ü�̴�.
class MySimpleRegex {
private: MySimpleRegex();
    //�� Ŭ������ static �޼��常 �����Ƿ� �����ڸ� private�� �д�.

public:
    class compiled; //��¥ ����� �ϴ� Ŭ����
    
    //����ǥ������ �������� ��ü compiled�� ��ȯ�Ѵ�.
    static compiled compile( const string& m_regex); 

    //source���� ����ǥ���İ� ���� ���� ��ġ�ϴ� ������ ���Ѵ�.
    //�׷� ������ ������ invalid�� ��ü�� ��ȯ�Ѵ�.
    static ranged_string match(const string& m_regex, const string& source);

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

//TODO: ���Ƿ��� �ִ� ��ü�� ����Ʈ���� �� Ư���� ���� �����ϱ�
// T* ���� ���̽� �����غ���
class MySimpleRegex::compiled {
public:
    //�־��� ����ǥ������ �Ľ��� ���������� ���±�踦 �����Ѵ�.
    compiled(const string& m_regex);

    //������ ���±�踦 �����Ѵ�. �޸� ��ȯ �۾��� �����Ѵ�.
    ~compiled();

    //source���� start_idx���� Ž���� ������ ���� ó������ �߰ߵ� ��ġ ������ ��ȯ�Ѵ�.
    ranged_string match(const string& source, unsigned index_start = 0) {
        return state_machine_input(source, index_start, false);
    }

    //source���� �߰ߵǴ� ��� ��ġ ������ ��ȯ�Ѵ�.
    //�ƹ� ������ ��ġ���� ������ empty() �� vector�� ��ȯ�ȴ�.
    vector<ranged_string> match_all(const string& source) {
        vector<ranged_string> ret;
        unsigned found_idx = 0;
        while (found_idx < source.size()) {
            ranged_string ans = match(source, found_idx);
            if (!ans.is_valid) break;
            ret.push_back(ans);
            found_idx = ret.back().end;
        }
        return ret;
    }



private:
    /****************************/
    /*      Inner Classes       */
    /****************************/

    //input char�� �Ǻ��ϱ� ���� ��ġ Ŭ����
    //������� �������� �����Ѵ�.
    class Imatchable;       //��ġ ��ü �������̽�    
    class matcher_single;   //���� ���� ��ġ    
    class matcher_dot;      //��� ���� ��ġ(���� ���� ����)
    class matcher_true;     //������ ��ġ


    //���¸ӽ��� ���� ��� �����͸� ǥ���ϴ� Ŭ�����̴�    
    //node�� ����Ű�� node_ptr �߻� Ŭ������ �Ļ� Ŭ������ �����ȴ�.
    //Ȱ��ȭ ��ȣ�� �ٷ� �����ϴ� node_ptr_direct ������,
    //���ο� ī���Ͱ� �־� ī��Ʈ�� ���� �̻��� �Ǹ� Ȱ��ȭ ��ȣ�� �����ϴ�
    //node_ptr_cnt_inner �����Ͱ� �����Ѵ�.
    class node;         //���¸ӽ��� ���
    class node_ptr;         
    class node_ptr_direct;  
    class node_ptr_inner_counter;


    //���� Ȱ��ȭ ��忡 ���� ������, ���� Ȱ��ȭ ��� ����� target��, 
    //Ȱ��ȭ ���¿� ���� ���̵Ǵ� ������ start_index ������ ��´�.
    struct active_request_info {
        node* target;
        unsigned start_index;
    };

    //state machine ������
    class state_machine_creator;


    /****************************/
    /*      Private fields      */
    /****************************/
    vector<node*> m_node;   //���±�� ��� �����̳� (���� ������)
    node* m_epsilon;        //���Ƿ� ��ȣ�� next�� �ִ� ���
    node* m_terminal;       //�͹̳� ��� ����Ʈ (���� �����)
    const string m_regex;   //����� ����ǥ����


    /****************************/
    /*    Private functions     */
    /****************************/ 
    ranged_string state_machine_input(      //���±�迡 ���ڿ��� �Է����� �־��ָ�
        const string& src,                  //���� ó������ accept�� ��ġ ������ ����Ѵ�.
        unsigned index_start = 0,           //��ġ�� ������ ������ invalid�� ��ü�� ��ȯ�Ѵ�.
        bool check_at_front_only = false);
         
    void request_active(    //active ��ų ��带 active_list�� ����Ѵ�.
        map<string, node*>& actives, node* to_active, unsigned state_istart);

};



} //end of namespace assignment1
#endif