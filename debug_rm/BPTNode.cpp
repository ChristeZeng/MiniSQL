#include "BPTNode.h"
#include <iostream>
using namespace std;

/*创建一个空白结点*/
template <class T>
BPTNode<T>::BPTNode(string filename, int BlockId, int KeyLen, int parent, int order, bool isLeaf, BufferManager* bm)
{
    //cout << "创建一个全新结点" << endl;
    this->bm = bm;
    this->filename = filename;          //关联的树名
    this->BlockId = BlockId;           //它位于文件的offset
    this->KeyLen = KeyLen;
    this->parent = parent;
    this->order = order;
    this->isLeaf = isLeaf;
    this->mode = 2;
    LeftNode = RightNode = -1;
    if (isLeaf)
        children.push_back(-1);

}

/*复制一个结点，作为指针使用，修改完内部值在解构时写回文件*/
template <class T>
BPTNode<T>::BPTNode(string filename, int offset, int KeyLen, int order, BufferManager* bm)
{

    //cout << "正在复制读取节点" << endl;

    this->bm = bm;
    this->BlockId = offset;
    this->KeyLen = KeyLen;
    this->order = order;
    this->filename = filename;

    int Id = bm->ReadIndexFromFile(filename, offset);
    bm->PinBlock(Id);
    char* buf = bm->ReadBlockFromFile(Id);
    parent = *(reinterpret_cast<int*>(buf + 0));
    LeftNode = *(reinterpret_cast<int*>(buf + 4));
    RightNode = *(reinterpret_cast<int*>(buf + 8));
    int num = *(reinterpret_cast<int*>(buf + 12));     //键值数目
    children.push_back(*(reinterpret_cast<int*>(buf + 16)));
    isLeaf = children[0] == -1 ? 1 : 0;

    int position = 20;
    for (int i = 0; i < num; i++)
    {
        if constexpr (is_same<T, string>())
            keys.push_back(string(reinterpret_cast<char*>(buf + position)));
        else
            keys.push_back(*reinterpret_cast<T*>(buf + position));

        if (isLeaf)
        {
            record.push_back(*reinterpret_cast<int*>(buf + position + KeyLen));
            isRemoved.push_back(*reinterpret_cast<int*>(buf + position + KeyLen + 4)); //是不是要对齐
            position += 4 + KeyLen + 4;
        }
        else
        {
            children.push_back(*reinterpret_cast<int*>(buf + position + KeyLen));
            position += 4 + KeyLen;
            //isRemoved.push_back( *reinterpret_cast<bool*>(bm->buffer_pool[BlockId].buf + offset + KeyLen + sizeof(bool)));
        }

    }
    bm->UnPinBlock(Id);
}

template<class T>
BPTNode<T>::~BPTNode()
{
    /*
    cout << "this is ~BPTNode"  << " mode = " << mode << endl;
    if (mode != 0)   //如果mode == 0 将当前节点丢弃
    {
        int Id = bm->ReadIndexFromFile(filename, BlockId);
        bm->PinBlock(Id);
        char* buf = bm->ReadBlockFromFile(Id);
        //仅仅将头信息写回文件
        if (mode == 1)
        {
            memcpy(buf + 0, &parent, 4);
            memcpy(buf + 4, &LeftNode, 4);
            memcpy(buf + 8, &RightNode, 4);
            memcpy(buf + 16, &children[0], 4);
        }
        //写回所有信息
        else if (mode == 2)
        {
            int num = keys.size();
            cout << endl;
            cout << "____________写入结点信息__________" << endl;
            cout << "正在写入文件，位置 " << BlockId << "申请到的空间 " << Id << "记录数 " << num << endl;
            for (int i = 0; i < keys.size(); i++)
                cout << keys[i] << " ";
            cout << endl;

            memcpy(buf + 0, &parent, 4);
            memcpy(buf + 4, &LeftNode, 4);
            memcpy(buf + 8, &RightNode, 4);
            memcpy(buf + 12, &num, 4);
            memcpy(buf + 16, &children[0], 4);
            int postion = 20;
            for (int i = 0; i < keys.size(); i++)
            {
                if constexpr (is_same<T, string>())
                {
                    memcpy(buf + postion, keys[i].c_str(), KeyLen);
                }
                else
                    memcpy(buf + postion, &keys[i], KeyLen);

                if (isLeaf)
                {
                    memcpy(buf + postion + KeyLen, &record[i], 4);
                    memcpy(buf + postion + KeyLen + 4, &isRemoved[i], 4);
                    postion += KeyLen + 4 + 4;
                }
                else
                {
                    memcpy(buf + postion + KeyLen, &children[i + 1], 4);
                    //memcpy(bm->buffer_pool[Id].buf + postion + KeyLen + 4, isRemoved[i], 1);
                    postion += KeyLen + 4;
                }
            }

        }
        bm->DirtyBlock(Id);
        bm->writetofile(Id);
        bm->UnPinBlock(Id);
    }
    else
        return;
        */
}