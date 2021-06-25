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
    string filename;                                     //����index���ļ���
    int typeLen;                                         //���������͵Ĵ�С(���ڼ������)
    BufferManager* bm;
public:
    IndexManager(string IndexName, string TableName, string AttributeName, int typeLen, BufferManager* bm);

    void createIndex();
    bool dropIndex();
    int searchviakey(const T& key);                        //��ֵ���ң����ز��ҵ���ֵ��buffer�еı��(0-4095)
    vector<int> rangesearch(const T& beg, const T& end); //��Χ���ң����ر�ŵļ���
    bool insertkey(const T& key, const int addr);                        //����һ����ֵ
    bool deletekey(const T& key);                        //ɾ��һ����ֵ
    void initialize(vector<T> keys, vector<int> records);
    void print();
    void printBPT();                                     //��ӡ������Debug
};
