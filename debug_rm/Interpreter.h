#ifndef __H_INTER_H__
#define  __H_INTER_H__
#include<iostream>
#include<vector>
#include<string>
#include "Defines.h"
#include "Attribute.h"
#include "Tuple.h"
#include "Condition.h"

#include "API.h"
using namespace std;

class Interpreter
{
public:
    string sql;
    vector<string> words;
    SQL_TYPE sql_type;
    API api;
    void into_one_line();
    void make_regular();
    void into_words();
    int get_type();
    void execfile(const string& f_name);

    Interpreter();
    ~Interpreter();
    void get_sql(const string& sql);
    int run_sql();
    void print();
    void printword();
};

#endif