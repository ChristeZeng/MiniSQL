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
    vector<Tuple> tuple;      //存放元组
    vector<Attribute> attri;  //存放属性
    vector<Index> index;      //存放索引
    int attriCnt;             //属性数量
    int tupleCnt;             //元组数量
    int indexCnt;             //索引数量
    int blockNum;
};

#endif