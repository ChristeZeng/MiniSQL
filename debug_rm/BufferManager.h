#pragma once

#include <string>
#include "Block.h"
using namespace std;
const int Buffer_MAX = 100;  //buffer�������Դ�Ŷ��ٿ�

class BufferManager
{
private:
    Block* buffer_pool;
    int getBufferEmpty();
public:


    BufferManager();
    ~BufferManager();

    void PinBlock(int BlockId);                                    //�ṩ������ҳ��pin���ܣ���������������ҳ���������滻��ȥ
    void UnPinBlock(int BlockId);                                  //�������ݿ�
    bool IsDirtyBlock(int BlockId);                                //��¼�������и�ҳ��״̬�Ƿ��޸Ĺ�
    void DirtyBlock(int BlockId);                                  //���޸ı�Ǽ�Ϊ1
    int ReadIndexFromFile(const string& filename, int offset);     //��ȡ�ļ���һ��buffer���У������ؿ�ı��
    char* ReadBlockFromFile(int BlockId);                          //��ȡ��BlockID��Ӧ��pool��buf
    bool writetofile(const int BlockId);                           //д��һ������ļ�
    void FreeBlock(const string& filename);                        //�ͷ�һ�����

    char* FindLastIndex(int index);                                //�ҵ�������block�л�û�б��޸Ĺ���Ԫ����С�±�
    bool writetofile(Block* b);                                    //����д���ļ�һ����
    Block FindBlock(int BlockId);                                  //����buffer_pool���±�ΪBlockId��Blockָ��
    void UpdateBuffer_pool(Block* buffer_temp);                    //����buffer_pool
};

//extern BufferManager *bm;                                      