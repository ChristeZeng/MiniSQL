#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "BufferManager.h"
#include "BPlusTree.h"
using namespace std;

template <class T>
class IndexManager
{
private:
    string filename;                                     //存入index的文件名
    int typeLen;                                         //索引键类型的大小(用于计算叉数)
    BufferManager* bm;
public:
    IndexManager(string IndexName, string TableName, string AttributeName, int typeLen, BufferManager* bm);

    void createIndex();
    bool dropIndex();
    int searchviakey(const T& key);                        //等值查找，返回查找到的值在buffer中的编号(0-4095)
    vector<int> rangesearch(const T& beg, const T& end); //范围查找，返回编号的集合
    bool insertkey(const T& key, const int addr);                        //插入一个键值
    bool deletekey(const T& key);                        //删除一个键值
    void initialize(vector<T> keys, vector<int> records);
    void print();
    void printBPT();                                     //打印树方便Debug
};
