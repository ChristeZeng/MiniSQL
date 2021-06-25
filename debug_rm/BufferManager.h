#pragma once

#include <string>
#include "Block.h"
using namespace std;
const int Buffer_MAX = 100;  //buffer中最多可以存放多少块

class BufferManager
{
private:
    Block* buffer_pool;
    int getBufferEmpty();
public:


    BufferManager();
    ~BufferManager();

    void PinBlock(int BlockId);                                    //提供缓冲区页的pin功能，及锁定缓冲区的页，不允许替换出去
    void UnPinBlock(int BlockId);                                  //解锁数据块
    bool IsDirtyBlock(int BlockId);                                //记录缓冲区中各页的状态是否被修改过
    void DirtyBlock(int BlockId);                                  //将修改标记记为1
    int ReadIndexFromFile(const string& filename, int offset);     //读取文件到一个buffer块中，并返回块的编号
    char* ReadBlockFromFile(int BlockId);                          //读取到BlockID对应的pool的buf
    bool writetofile(const int BlockId);                           //写回一个块给文件
    void FreeBlock(const string& filename);                        //释放一个块空

    char* FindLastIndex(int index);                                //找到给定的block中还没有被修改过的元素最小下标
    bool writetofile(Block* b);                                    //重载写给文件一个快
    Block FindBlock(int BlockId);                                  //返回buffer_pool中下标为BlockId的Block指针
    void UpdateBuffer_pool(Block* buffer_temp);                    //更新buffer_pool
};

//extern BufferManager *bm;                                      