#ifndef _H_API_H
#define _H_API_H

#include<vector>
#include<iostream>
#include"Attribute.h"
#include"Condition.h"
#include"CatalogManager.h"
#include"RecordManager.h"
#include"BufferManager.h"
#include"IndexManager.h"
#include"BPlusTree.h"
#include"BPTNode.h"
#include"IndexManager.cpp"
#include"BPlusTree.cpp"
#include"BPTNode.cpp"
using namespace std;

class API {
    CatalogManager* cm;
    RecordManager* rm;
    BufferManager* bm;

public:
    API();
    ~API();

    void TableDrop(string TableName);//drop table
    void TableCreate(string TableName, vector<Attribute>attribute);
    void TableCreate(string TableName, vector<Attribute>attribute, string primary, int primary_location);//create table according to the name,attribute and primary key

    void IndexDrop(string indexName);//drop index
    void IndexCreate(string indexName, string TableName, string attributeName); //create index for sertain attribute

    void SelectData(string TableName, vector<Attribute>* attr); //select without condition
    void SelectData(string TableName, vector<Attribute>* attr, vector<Condition>* con); //select with condition

    void InsertTuple(string TableName, Tuple record);//Insert one tuple to the table

    void DeleteTuple(string TableName);//delete all tuple of the table
    void DeleteTuple(string TableName, vector<Condition>* con);//delete tuple according to the condition



    bool ExistTable(string TableName);
    bool AttributeGet(string TableName, vector<Attribute>* attr);
    void AttributeShow(vector<Attribute>attr);
    bool CheckAttribute(vector<Attribute>attr,vector<Attribute>all);
    vector<int> Index_Condition_Check(Condition con, Table t);
    template<typename T>
    vector<int> RangeSearch(IndexManager<T>im, Condition con, Table t, T value);
    vector<int> No_Index_Condition_Check(Condition con, Table t);
    vector<int>Combine_two(vector<int> a, vector<int>b);
    int min_vector(int a, int b);
    vector<int> Unique_Tuple(vector<int> few, vector<int>large);
    void FindAttributeIndex(vector<Index>temp,vector<string>*a,Table *t);
    void FindIndexName(vector<Index>temp,vector<string>*a);
    void UpdateIndex(int offset,Index id, Table t,Tuple record);
};

#endif
