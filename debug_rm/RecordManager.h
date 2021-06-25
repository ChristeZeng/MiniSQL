#pragma once
#ifndef __H_RECORDMANAGER_H__
#define __H_RECORDMANAGER_H__

#include <vector>
#include<variant>
#include "BufferManager.h"
#include "Table.h"
#include "Condition.h"
#include "Index.h"
#include <boost/variant.hpp>

using namespace std;
using value = boost::variant<int, float, string>;

class RecordManager
{
private:
    BufferManager* bm;
public:
    RecordManager(BufferManager * bm_temp);
    ~RecordManager();
    void CreateTable(string table); //create table 
    bool DropTable(string table);   //drop table
    int InsertRecord(Table& table, Tuple& tuple);      //insert one tuple
    void DeleteRecord(Table& table, Tuple& tuple);      //delete one tuple
    vector<Tuple> SelectRecord(const Table& table, const Condition& con);   //select tuples satisfying the requirement
    vector<value> FindValue(const Table& table, string IndexName);
    vector<int> FindValueOffset(const Table& table, string IndexName);
    bool RecordConditionCheck(const Table& table, vector<Attribute>attr, vector<Condition>con); //check the table with certain condition bound 
    bool TupleConditionCheck(const Table& table, char* p, int offset, vector<Condition>con); //check one tuple with the certain conditon bound and output the result
    int FindAttributeOffset(const Table& table, Attribute attr);
    int GetBlockNum(const Table& table);
    bool RecordDeleteCondition(const Table& table, vector<Condition>con);



    bool checkUnique(Table& table, int i, Tuple& tuple); //check the unique requirement
    int Getlen(const Table table);//get the length of tuple
    int Caculate_Offset(const Table table, int n);
    bool InsertTuple(Table& table, Tuple& tuple, char* p);
    void check_and_insert(char* p, int offset, int pre_offset, int type_attr, const Table& table, const Condition& con, vector<Tuple>& ret);
    Tuple getTuple(char* p, const Table& table);
    int GetAttributeSize(Attribute attr);
    template<typename T>
    bool ConditionCheck(const T& a, const T& b, const Condition& con);
    void UpdateBufferManager(BufferManager* bm);
    void ShowData(vector<int>offset, Table* t);
};

#endif