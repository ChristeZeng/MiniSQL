#pragma once
#include <string>
#include <vector>
#include "BufferManager.h"
using namespace std;

template<class T>
class BPTNode
{
public:
    string filename;          //�����ĸ��ļ���BPT
    int KeyLen;               //������Ĵ�С
    int parent;               //ָ���׵�buffermanager���
    int order;                //BPT�Ĳ���


    BufferManager* bm;
    int BlockId;
    int LeftNode;
    int RightNode;
    bool isLeaf;              //�Ƿ�ΪҶ�ӽ��
    int mode;                 //�⹹ģʽ
    vector<T> keys;           //�˽��ļ�ֵ
    vector<int> children;     //���ӵ�Block���
    vector<int> record;       //���һ�㣬������Ϣoffset;
    vector<int> isRemoved;   //αɾ���ı��

    BPTNode(string filename, int BlockId, int KeyLen, int parent, int order, bool isLeaf, BufferManager* bm);     //�������
    BPTNode(string filename, int BlockId, int KeyLen, int order, BufferManager* bm);                              //����һ�����
    ~BPTNode();                                                                                //����ģʽmodeȷ����ν⹹
};


