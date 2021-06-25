#ifndef __H_TABLE_H__
#define __H_TABLE_H__

#include <string>
#include "Attribute.h"
#include "Tuple.h"
#include "Index.h"
using namespace std;

class Table
{
public:
    string name;
    vector<Tuple> tuple;      //���Ԫ��
    vector<Attribute> attri;  //�������
    vector<Index> index;      //�������
    int attriCnt;             //��������
    int tupleCnt;             //Ԫ������
    int indexCnt;             //��������
    int blockNum;
};

#endif