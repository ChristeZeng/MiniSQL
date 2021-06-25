#pragma once
#include "BPTNode.h"
using namespace std;

template <class T>
class BPlusTree
{
private:
    string filename;           //�������ļ���
    int KeyLen;                //��ֵ���ͳ���      

    int order;                 //BPT�Ľ���
    int NumofBlock;            //BPT��Ҫ�����ڼ���block��
    int NextWriteBlock;        //BPT�����һ����λ�õ�Block(���������)
    BufferManager* bm;
public:
    int root;                  //������Block�ı��
    BPlusTree(const string& filename, BufferManager* bm);
    int initialize(vector<T>& key, vector<int>& record);              //��ʼ��B+���������ظ���Block���
    int Find(const T& key);        //���ص���key�������ļ��е�ƫ����
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
