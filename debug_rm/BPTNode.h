#pragma once
#include <string>
#include <vector>
#include "BufferManager.h"
using namespace std;

template<class T>
class BPTNode
{
public:
    string filename;          //属于哪个文件的BPT
    int KeyLen;               //所存键的大小
    int parent;               //指向父亲的buffermanager编号
    int order;                //BPT的叉数


    BufferManager* bm;
    int BlockId;
    int LeftNode;
    int RightNode;
    bool isLeaf;              //是否为叶子结点
    int mode;                 //解构模式
    vector<T> keys;           //此结点的键值
    vector<int> children;     //孩子的Block编号
    vector<int> record;       //最后一层，真正信息offset;
    vector<int> isRemoved;   //伪删除的标记

    BPTNode(string filename, int BlockId, int KeyLen, int parent, int order, bool isLeaf, BufferManager* bm);     //创建结点
    BPTNode(string filename, int BlockId, int KeyLen, int order, BufferManager* bm);                              //复制一个结点
    ~BPTNode();                                                                                //根据模式mode确定如何解构
};


