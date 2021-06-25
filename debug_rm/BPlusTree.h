#pragma once
#include "BPTNode.h"
using namespace std;

template <class T>
class BPlusTree
{
private:
    string filename;           //所存树文件名
    int KeyLen;                //键值类型长度      

    int order;                 //BPT的阶数
    int NumofBlock;            //BPT需要被存在几个block里
    int NextWriteBlock;        //BPT的最后一个有位置的Block(方便插入结点)
    BufferManager* bm;
public:
    int root;                  //根所在Block的编号
    BPlusTree(const string& filename, BufferManager* bm);
    int initialize(vector<T>& key, vector<int>& record);              //初始化B+树，并返回根的Block编号
    int Find(const T& key);        //返回的是key在索引文件中的偏移量
    vector<int> FindRange(const T& beg, const T& end);
    bool Insert(const T& key, int addr);
    bool Delete(const T& key);
    bool FakeDelete(const T& key);
    void printtree(int xid, int d);

    int FindWithKey(const T& key);
    vector<int> FindRangeWithKey(const T& begin, const T& end);

    void writebacknode(BPTNode<T>* Node);
    BPTNode<T>* FindNode(const T& key);
    BPTNode<T>* splitLeafNode(BPTNode<T>* Node);
    BPTNode<T>* splitNonLeafNode(BPTNode<T>* Node);
};
