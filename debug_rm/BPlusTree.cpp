#include "BPlusTree.h"
#include <cmath>
#include <iostream>
using namespace std;

/*����һ������*/
template<class T>
BPlusTree<T>::BPlusTree(const string& filename, BufferManager* bm)
{
    this->bm = bm;
    this->filename = filename;
    /*���ļ�ͷ��ȡ��������Ϣ*/
    int BlockId = bm->ReadIndexFromFile(filename, 0);
    bm->PinBlock(BlockId);
    char* buf = bm->ReadBlockFromFile(BlockId);
    order = *(reinterpret_cast<int*>(buf + 0));
    KeyLen = *(reinterpret_cast<int*>(buf + 4));
    NumofBlock = *(reinterpret_cast<int*>(buf + 8));
    root = *(reinterpret_cast<int*>(buf + 12));
    NextWriteBlock = *(reinterpret_cast<int*>(buf + 16));


    //cout << order << " " << KeyLen << " " << NumofBlock << " " << root << " " << NextWriteBlock << endl;
    /*���֮ǰû�н���������Ҫ��ʼ������*/
    if (root == -1 && NumofBlock == 0)
    {
        //cout << "���ڽ���" << endl;
        root = 1;
        NumofBlock = 1;
        NextWriteBlock = 1;    //ͷ��Ϣ����ռ��һ����Ĵ�С
        BPTNode<T>* p = new BPTNode<T>(filename, NextWriteBlock, KeyLen, -1, order, true, bm);
        NextWriteBlock++;      //�½��ڵ����һ��block��ʼ
        writebacknode(p);
        delete p;
    }

    //�������ļ�Ͷ��Ϣ
    memcpy(buf + 8, &NumofBlock, 4);
    memcpy(buf + 12, &root, 4);
    memcpy(buf + 16, &NextWriteBlock, 4);
    bm->DirtyBlock(BlockId);
    bm->UnPinBlock(BlockId);
    //count << "��ȡ����������Ϣ" << endl;

}

template<class T>
int BPlusTree<T>::initialize(vector<T>& key, vector<int>& record)
{
    /*�ӱ��ļ��ж�ȡ�����е���������Ϣ������һһ����B+���������ļ�����һ����Ҫ��record��Ϣ�ĵ���*/
    auto it1 = key.begin();
    auto it2 = record.begin();
    int i = 0;
    while (it1 != key.end() && it2 != record.end())
    {
        Insert(*it1, *it2);
        it1++;
        it2++;
        i++;
        //cout << "---------��ʱ����Ϊ-------" << endl;
        //printtree(root, 0);
    }
    return root;
}


template<class T>
BPTNode<T>* BPlusTree<T>::splitLeafNode(BPTNode<T>* Node)
{
    //cout << "-----------���ڷָ���------------" << endl;
    //���ļ���NextWriteBlockλ���½�һ����㣬�����Ϣ�ɲ����ṩ

    BPTNode<T>* NewNode = new BPTNode<T>(filename, NextWriteBlock, KeyLen, Node->parent, order, Node->isLeaf, bm);
    NextWriteBlock++;
    NumofBlock++;
    Node->mode = 2;
    int length = Node->keys.size();
    //�����޸���Ϣ
    for (int i = ceil((double)length / 2.0); i < length; i++)
    {
        NewNode->keys.push_back(Node->keys[ceil((double)length / 2.0)]);
        NewNode->record.push_back(Node->record[ceil((double)length / 2.0)]);
        NewNode->isRemoved.push_back(Node->isRemoved[ceil((double)length / 2.0)]);

        Node->keys.erase(Node->keys.begin() + ceil((double)length / 2.0));
        Node->record.erase(Node->record.begin() + ceil((double)length / 2.0));
        Node->isRemoved.erase(Node->isRemoved.begin() + ceil((double)length / 2.0));
    }

    //ֻά�����ҽڵ����Ϣ
    NewNode->RightNode = Node->RightNode;
    Node->RightNode = NewNode->BlockId;

    //�������Ҷ�ӽ�����Ƕ���
    if (Node->parent == -1)
    {
        BPTNode<T>* Newparent = new BPTNode<T>(filename, NextWriteBlock, KeyLen, -1, order, false, bm);
        Newparent->mode = 2;
        NextWriteBlock++;
        NumofBlock++;
        Newparent->keys.push_back(NewNode->keys[0]);
        Newparent->record.push_back(NewNode->record[0]);

        Newparent->children.push_back(Node->BlockId);
        Newparent->children.push_back(NewNode->BlockId);
        Node->parent = Newparent->BlockId;
        NewNode->parent = Newparent->BlockId;
        root = Newparent->BlockId;

        Node->mode = 2;
        writebacknode(Node);
        writebacknode(NewNode);
        writebacknode(Newparent);
        delete Node;
        delete NewNode;
        return Newparent;
    }
    //������Ҷ�ӽ��
    BPTNode<T>* Newparent = new BPTNode<T>(filename, NewNode->parent, KeyLen, order, bm);
    Newparent->mode = 2;
    auto iter = lower_bound(Newparent->keys.begin(), Newparent->keys.end(), NewNode->keys[0]);
    //�ڽ���ֵ����ĩβ
    if (iter == Newparent->keys.end())
    {
        Newparent->keys.push_back(NewNode->keys[0]);
        Newparent->children.push_back(NewNode->BlockId);
        writebacknode(Node);
        writebacknode(NewNode);
        writebacknode(Newparent);
        delete Node;
        delete NewNode;
        return Newparent;
    }
    auto differ = iter - Newparent->keys.begin();
    Newparent->keys.insert(iter, NewNode->keys[0]);
    Newparent->children.insert(differ + Newparent->children.begin() + 1, NewNode->BlockId);
    writebacknode(Node);
    writebacknode(NewNode);
    writebacknode(Newparent);
    delete Node;
    delete NewNode;
    return Newparent;
}

/*��Ҷ�ӽ��ķ���*/
template <class T>
BPTNode<T>* BPlusTree<T>::splitNonLeafNode(BPTNode<T>* Node)
{
    //cout << "------------��Ҷ�ӽ��ķ���--------------" << endl;
    BPTNode<T>* NewNode = new BPTNode<T>(filename, NextWriteBlock, KeyLen, Node->parent, order, Node->isLeaf, bm);

    NextWriteBlock++;
    NumofBlock++;


    //Debug;
    BPTNode<T>* p = NewNode;
    //Debug

    int length = Node->keys.size();
    int mid = ceil((double)(order - 1.0) / 2.0);
    T midkey = Node->keys[mid];
    //�����޸���Ϣ
    for (int i = mid + 1; i < length; i++)
    {
        NewNode->keys.push_back(Node->keys[mid + 1]);
        NewNode->children.push_back(Node->children[mid + 1]);

        BPTNode<T>* Childptr = new BPTNode<T>(filename, Node->children[mid + 1], KeyLen, order, bm);
        Childptr->parent = NewNode->BlockId;
        Childptr->mode = 1;
        writebacknode(Childptr);
        delete Childptr;

        Node->keys.erase(Node->keys.begin() + mid + 1);
        Node->children.erase(Node->children.begin() + mid + 1);
    }

    NewNode->children.push_back(Node->children[mid + 1]);
    BPTNode<T>* Childptr = new BPTNode<T>(filename, Node->children[mid + 1], KeyLen, order, bm);
    Childptr->parent = NewNode->BlockId;
    Childptr->mode = 1;
    writebacknode(Childptr);
    delete Childptr;

    Node->children.erase(Node->children.begin() + mid + 1);
    Node->keys.erase(Node->keys.begin() + mid);

    //ֻά�����ҽڵ����Ϣ
    NewNode->RightNode = Node->RightNode;
    Node->RightNode = NewNode->BlockId;

    //������Ǹ����
    if (Node->parent == -1)
    {
        BPTNode<T>* Newparent = new BPTNode<T>(filename, NextWriteBlock, KeyLen, -1, order, false, bm);

        NextWriteBlock++;
        NumofBlock++;

        Newparent->keys.push_back(midkey);
        Newparent->children.push_back(Node->BlockId);
        Newparent->children.push_back(NewNode->BlockId);

        Node->parent = Newparent->BlockId;
        NewNode->parent = Newparent->BlockId;
        root = Newparent->BlockId;

        NewNode->mode = 2;
        Node->mode = 2;
        Newparent->mode = 2;
        writebacknode(Node);
        writebacknode(NewNode);
        writebacknode(Newparent);

        delete Node;
        delete NewNode;
        return Newparent;
    }

    BPTNode<T>* Newparent = new BPTNode<T>(filename, Node->parent, KeyLen, order, bm);
    auto iter = lower_bound(Newparent->keys.begin(), Newparent->keys.end(), midkey);

    auto differ = iter - Newparent->keys.begin();

    Newparent->keys.insert(iter, midkey);
    Newparent->children.insert(differ + Newparent->children.begin() + 1, NewNode->BlockId);

    Node->mode = 2;
    NewNode->mode = 2;
    Newparent->mode = 2;
    writebacknode(Node);
    writebacknode(NewNode);
    writebacknode(Newparent);

    delete Node;
    delete NewNode;
    return Newparent;
}

/*�ҵ�keyֵ���ڵ�Ҷ�ӽ��*/
template<class T>
BPTNode<T>* BPlusTree<T>::FindNode(const T& key)
{
    int pointer = root;
    BPTNode<T>* p = new BPTNode<T>(filename, root, KeyLen, order, bm);
    int nextNode;
    if (p->isLeaf) nextNode = -1;
    else
    {
        auto iter = lower_bound(p->keys.begin(), p->keys.end(), key);
        if (iter == p->keys.end())
            nextNode = p->children.back();
        else
            nextNode = (key < *iter) ? p->children[iter - p->keys.begin()] : p->children[iter - p->keys.begin() + 1];
    }

    while (nextNode != -1)
    {
        pointer = nextNode;
        delete p;
        p = new BPTNode<T>(filename, pointer, KeyLen, order, bm);

        if (p->isLeaf) nextNode = -1;
        else
        {
            auto iter = lower_bound(p->keys.begin(), p->keys.end(), key);
            if (iter == p->keys.end())
                nextNode = p->children.back();
            else
                nextNode = (key < *iter) ? p->children[iter - p->keys.begin()] : p->children[iter - p->keys.begin() + 1];
        }
    }

    p->mode = 0;
    return p;
}

template<class T>
int BPlusTree<T>::Find(const T& key)
{
    int pointer;
    BPTNode<T>* p = new BPTNode<T>(filename, root, KeyLen, order, bm);
    while (p->children[0] != -1)
    {
        int i = 0;
        for (i = 0; i < p->keys.size(); i++)
        {
            if (key < p->keys[i])
                break;
        }
        if (i == p->keys.size())
            pointer = p->children.back();
        else
            pointer = p->children[i];

        delete p;
        p = new BPTNode<T>(filename, pointer, KeyLen, order, bm);
    }

    //cout << p->keys[0] << endl;
    auto iter = lower_bound(p->keys.begin(), p->keys.end(), key);
    if (iter != p->keys.end() && *iter == key)
        return p->record[iter - p->keys.begin()];

    return -1;
}

/*��Χ���ң����ر��*/
template <class T>
vector<int> BPlusTree<T>::FindRange(const T& beg, const T& end)
{
    vector<int> ret;
    BPTNode<T>* begin = FindNode(beg);
    BPTNode<T>* endno = FindNode(end);
    BPTNode<T>* p;

    /*���ͬ��һ����㣬˵�����������Χ��*/
    if (begin->BlockId == endno->BlockId)
    {
        p = begin;
        auto cur1 = lower_bound(p->keys.begin(), p->keys.end(), beg);
        auto cur2 = upper_bound(p->keys.begin(), p->keys.end(), end);

        int differ;
        for (auto iter = cur1; iter != cur2; ++iter)
        {
            differ = iter - p->keys.begin();
            ret.push_back(p->record[differ]);
        }

        delete begin;
        delete endno;
        return ret;
    }
    /*����*/
    else
    {
        /*���½�������з���Ҫ��ļ�λ��ѹ��*/
        p = begin;
        auto cur1 = lower_bound(p->keys.begin(), p->keys.end(), beg);
        for (auto iter = cur1; iter < p->keys.end(); iter++)
            ret.push_back(p->record[iter - p->keys.begin()]);
        /*���м�λ�õ����н��ļ�ֵλ��ѹ��*/
        p = new BPTNode<T>(filename, p->RightNode, KeyLen, order, bm);   //ָ���ұ�һ��Ҷ�ӽ�� 
        while (p->BlockId != endno->BlockId)
        {
            for (auto iter = p->keys.begin(); iter < p->keys.end(); iter++)
                ret.push_back(p->record[iter - p->keys.begin()]);
            p = new BPTNode<T>(filename, p->RightNode, KeyLen, order, bm);
        }
        /*����������Ҫ��ļ�ֵλ��ѹ��*/
        auto cur2 = upper_bound(endno->keys.begin(), endno->keys.end(), end);
        for (auto iter = endno->keys.begin(); iter < cur2; iter++)
            ret.push_back(endno->record[iter - endno->keys.begin()]);

        delete begin;
        delete endno;
        return ret;
    }
}

template <class T>
bool BPlusTree<T>::Insert(const T& key, int addr)
{

    BPTNode<T>* insertnode = FindNode(key);
    auto cur = lower_bound(insertnode->keys.begin(), insertnode->keys.end(), key);


    //�Ѿ�����
    if (cur < insertnode->keys.end() && *cur == key)
    {
        if (insertnode->isRemoved[cur - insertnode->keys.begin()] != 1)
            return false;
        else
        {
            insertnode->isRemoved[cur - insertnode->keys.begin()] = 0;
            insertnode->record[cur - insertnode->keys.begin()] = addr;
            insertnode->mode = 2;
            writebacknode(insertnode);
            delete insertnode;
            return true;
        }
    }

    //ֱ�Ӳ����¼
    auto differ = cur - insertnode->keys.begin();
    insertnode->keys.insert(cur, key);
    insertnode->record.insert(differ + insertnode->record.begin(), addr);
    insertnode->isRemoved.insert(differ + insertnode->isRemoved.begin(), 0);

    //cout << "����ִ�в���!������ڵ���Ϣ" << endl;
    //cout << insertnode->keys[differ] << " " << insertnode->record[differ] << " " << insertnode->isRemoved[differ] << " " << insertnode->BlockId << " " << order << " " << insertnode->keys.size() << endl;


    //Ҷ�ӽ�������order����ֵ
    BPTNode<T>* p = insertnode;
    if (insertnode->keys.size() <= order)
    {
        //���½����Ϣ��BPTree��Ϣ������ļ�
        int BlockId = bm->ReadIndexFromFile(filename, 0);
        bm->PinBlock(BlockId);
        char* buf = bm->ReadBlockFromFile(BlockId);

        memcpy(buf + 0, &order, 4);
        memcpy(buf + 4, &KeyLen, 4);
        memcpy(buf + 8, &NumofBlock, 4);
        memcpy(buf + 12, &root, 4);
        memcpy(buf + 16, &NextWriteBlock, 4);

        bm->DirtyBlock(BlockId);
        bm->UnPinBlock(BlockId);
        insertnode->mode = 2;
        writebacknode(insertnode);
        delete insertnode;
        return true;
    }
    else
    {
        while (p->keys.size() >= order)
        {
            if (p->isLeaf)
                p = splitLeafNode(p);
            else
                p = splitNonLeafNode(p);

            int BlockId = bm->ReadIndexFromFile(filename, 0);
            bm->PinBlock(BlockId);
            char* buf = bm->ReadBlockFromFile(BlockId);
            memcpy(buf + 0, &order, 4);
            memcpy(buf + 4, &KeyLen, 4);
            memcpy(buf + 8, &NumofBlock, 4);
            memcpy(buf + 12, &root, 4);
            memcpy(buf + 16, &NextWriteBlock, 4);
            bm->DirtyBlock(BlockId);
            bm->UnPinBlock(BlockId);
        }
    }

    delete p;
    int BlockId = bm->ReadIndexFromFile(filename, 0);
    bm->PinBlock(BlockId);
    char* buf = bm->ReadBlockFromFile(BlockId);
    memcpy(buf + 0, &order, 4);
    memcpy(buf + 4, &KeyLen, 4);
    memcpy(buf + 8, &NumofBlock, 4);
    memcpy(buf + 12, &root, 4);
    memcpy(buf + 16, &NextWriteBlock, 4);
    bm->DirtyBlock(BlockId);
    bm->UnPinBlock(BlockId);
    return true;
}

template <class T>
bool BPlusTree<T>::Delete(const T& key)
{
    BPTNode<T>* p = FindNode(key);
    if (p == NULL)
        return false;

    auto iter = lower_bound(p->keys.begin(), p->keys.end(), key);
    if (iter != p->keys.end() && *iter == key)
    {
        p->keys.erase(iter);
        p->record.erase(iter - p->keys.begin() + p->record.begin);
    }
    //û�������ֵ
    else
    {
        delete p;
        return false;
    }
    //����Ҫ������1.��Ҷ���� 2.������㹻
    if ((p->parent == -1 && p->isLeaf) || p->keys.size() >= floor((double)(order + 1.0) / 2))
    {
        delete p;
        int BlockId = bm->ReadIndexFromFile(filename, 0);
        bm->PinBlock(BlockId);
        memcpy(bm->buffer_pool[BlockId].buf, &order, 4);
        memcpy(bm->buffer_pool[BlockId].buf + 4, &KeyLen, 4);
        memcpy(bm->buffer_pool[BlockId].buf + 8, &NumofBlock, 4);
        memcpy(bm->buffer_pool[BlockId].buf + 12, &root, 4);
        memcpy(bm->buffer_pool[BlockId].buf + 16, &NextWriteBlock, 4);
        bm->writetofile(BlockId, filename, 0);
        bm->UnPinBlock(BlockId);
        return true;
    }
    //��Ҫ����
    BPTNode<T>* Parentptr = new BPTNode<T>(filename, p->parent, KeyLen, order);
    BPTNode<T>* Leftptr = NULL;
    BPTNode<T>* Rightptr = NULL;
    //�ҵ����Һ���
    if (p->LeftNode != -1)
        Leftptr = new BPTNode<T>(filename, p->LeftNode, KeyLen, order);
    if (p->RightNode != -1)
        Rightptr = new BPTNode<T>(filename, p->RightNode, KeyLen, order);
    //�������ͬ��һ����ĸ�ҹ����ֵ�������һ���ָ��˽��
    if (Leftptr && Leftptr->parent == p->parent && Leftptr->keys.size() >= floor((double)(order + 1.0) / 2.0))
    {
        p->keys.insert(p->keys.begin(), Leftptr->keys.back());
        p->record.insert(p->record.begin(), Leftptr->record.back());
        Leftptr->keys.pop_back();
        Leftptr->record.pop_back();
        return true;
    }
}

template <class T>
bool BPlusTree<T>::FakeDelete(const T& key)
{
    BPTNode<T>* p = FindNode(key);
    auto iter = lower_bound(p->keys.begin(), p->keys.end(), key);
    if (iter != p->keys.end() && *iter == key)
    {
        p->isRemoved[iter - p->keys.begin()] = 1;
        p->mode = 2;
        writebacknode(p);
        delete p;
        return true;
    }
    delete p;
    return false;
}

template <class T>
void BPlusTree<T>::printtree(int xid, int d)
{
    BPTNode<T>* x = new BPTNode<T>(filename, xid, KeyLen, order, bm);
    cout << "depth: " << d << " | ";
    for (auto el : x->keys) {
        cout << el << ' ';
    }
    cout << "\n";
    if (x->children[0] == -1) { return; }
    for (auto id : x->children) {
        printtree(id, d + 1);
    }
    if (x->children.empty()) {
        cout << "(";
        for (auto rec : x->record) {
            cout << rec << ", ";
        }
        cout << ")\n";
    }
    delete x;
}

template <class T>
void BPlusTree<T>::writebacknode(BPTNode<T>* Node)
{
    if (Node->mode != 0)   //���mode == 0 ����ǰ�ڵ㶪��
    {
        int Id = bm->ReadIndexFromFile(filename, Node->BlockId);
        bm->PinBlock(Id);
        char* buf = bm->ReadBlockFromFile(Id);
        //������ͷ��Ϣд���ļ�
        if (Node->mode == 1)
        {
            memcpy(buf + 0, &Node->parent, 4);
            memcpy(buf + 4, &Node->LeftNode, 4);
            memcpy(buf + 8, &Node->RightNode, 4);
            memcpy(buf + 16, &Node->children[0], 4);
        }
        //д��������Ϣ
        else if (Node->mode == 2)
        {
            int num = Node->keys.size();
            //cout << endl;
            //cout << "____________д������Ϣ__________" << endl;
            //cout << "����д���ļ���λ�� " << Node->BlockId << "���뵽�Ŀռ� " << Id << "��¼�� " << num << endl;
            //for (int i = 0; i < Node->keys.size(); i++)
                //cout << Node->keys[i] << " ";
            //cout << endl;

            memcpy(buf + 0, &Node->parent, 4);
            memcpy(buf + 4, &Node->LeftNode, 4);
            memcpy(buf + 8, &Node->RightNode, 4);
            memcpy(buf + 12, &num, 4);
            memcpy(buf + 16, &Node->children[0], 4);
            int postion = 20;
            for (int i = 0; i < Node->keys.size(); i++)
            {
                if constexpr (is_same<T, string>())
                {
                    memcpy(buf + postion, Node->keys[i].c_str(), KeyLen);
                }
                else
                    memcpy(buf + postion, &Node->keys[i], KeyLen);

                if (Node->isLeaf)
                {
                    memcpy(buf + postion + KeyLen, &Node->record[i], 4);
                    memcpy(buf + postion + KeyLen + 4, &Node->isRemoved[i], 4);
                    postion += KeyLen + 4 + 4;
                }
                else
                {
                    memcpy(buf + postion + KeyLen, &Node->children[i + 1], 4);
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
}

template <class T>
int BPlusTree<T>::FindWithKey(const T& key)
{
    int pointer;
    BPTNode<T>* p = new BPTNode<T>(filename, root, KeyLen, order, bm);
    while (p->children[0] != -1)
    {
        int i = 0;
        for (i = 0; i < p->keys.size(); i++)
        {
            if (key < p->keys[i])
                break;
        }
        if (i == p->keys.size())
            pointer = p->children.back();
        else
            pointer = p->children[i];

        delete p;
        p = new BPTNode<T>(filename, pointer, KeyLen, order, bm);
    }

    for (int i = 0; i < p->keys.size(); i++)
    {
        if (key == p->keys[i] && p->isRemoved[i] != 1)
            return p->record[i];
    }

    return -1;
}

template <class T>
vector<int> BPlusTree<T>::FindRangeWithKey(const T& beg, const T& end)
{
    vector<int> ret;
    BPTNode<T>* begin = FindNode(beg);
    BPTNode<T>* endno = FindNode(end);
    BPTNode<T>* p;

    /*���ͬ��һ����㣬˵�����������Χ��*/
    if (begin->BlockId == endno->BlockId)
    {
        p = begin;
        auto cur1 = lower_bound(p->keys.begin(), p->keys.end(), beg);
        auto cur2 = upper_bound(p->keys.begin(), p->keys.end(), end);

        int differ;
        for (auto iter = cur1; iter != cur2; ++iter)
        {
            differ = iter - p->keys.begin();
            if (p->isRemoved[differ] != 1)
                ret.push_back(p->record[differ]);
        }

        delete begin;
        delete endno;
        return ret;
    }
    /*����*/
    else
    {
        /*���½�������з���Ҫ��ļ�λ��ѹ��*/
        p = begin;
        auto cur1 = lower_bound(p->keys.begin(), p->keys.end(), beg);
        for (auto iter = cur1; iter < p->keys.end(); iter++)
        {
            if (p->isRemoved[iter - p->keys.begin()] != 1)
                ret.push_back(p->record[iter - p->keys.begin()]);
        }
        /*���м�λ�õ����н��ļ�ֵλ��ѹ��*/
        p = new BPTNode<T>(filename, p->RightNode, KeyLen, order, bm);   //ָ���ұ�һ��Ҷ�ӽ�� 
        while (p->BlockId != endno->BlockId)
        {
            for (auto iter = p->keys.begin(); iter < p->keys.end(); iter++)
            {
                int differ = iter - p->keys.begin();
                if (p->isRemoved[differ] != 1)
                    ret.push_back(p->record[iter - p->keys.begin()]);
            }
            p = new BPTNode<T>(filename, p->RightNode, KeyLen, order, bm);
        }
        /*����������Ҫ��ļ�ֵλ��ѹ��*/
        auto cur2 = upper_bound(endno->keys.begin(), endno->keys.end(), end);
        for (auto iter = endno->keys.begin(); iter < cur2; iter++)
        {
            if (endno->isRemoved[iter - endno->keys.begin()] != 1)
                ret.push_back(endno->record[iter - endno->keys.begin()]);
        }

        delete begin;
        delete endno;
        return ret;
    }
}