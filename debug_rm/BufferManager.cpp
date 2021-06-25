#include "BufferManager.h"
#include "Defines.h"
#include <iostream>
#include <cstring>
using namespace std;

/*申请空间，并且最多存Buffer_MAX块*/
BufferManager::BufferManager()
{
    buffer_pool = new Block[Buffer_MAX];
    for (int i = 0; i < Buffer_MAX; i++)
    {
        buffer_pool[i].file = "";
        buffer_pool[i].fileoffset = 0;
        buffer_pool[i].isdirty = false;
        buffer_pool[i].spaceused = false;
        buffer_pool[i].LRU_time = 0;
        memset(buffer_pool[i].buf, 0, sizeof(buffer_pool[i].buf));
    }
}

/*析构：1.将buffer中的块写回文件 2.释放空间*/
BufferManager::~BufferManager()
{
    //cout << "this is ~BufferManager" << endl;
    for (int i = 0; i < Buffer_MAX; i++)
    {
        if (buffer_pool[i].isdirty)
        {

            FILE* fp = fopen((buffer_pool[i].file + ".data").c_str(), "rb+");
            if (fp == NULL)
            {
                fp = fopen((buffer_pool[i].file + ".data").c_str(), "wb+");
                return;
            }
            memset(buffer_pool[i].buf, 0, MAX_BLOCK_SIZE);
            //cout << "第" << i << "个块需要写入文件位置" << buffer_pool[i].fileoffset << endl;
            fseek(fp, buffer_pool[i].fileoffset * MAX_BLOCK_SIZE, 0);
            fwrite(buffer_pool[i].buf, MAX_BLOCK_SIZE, 1, fp);
            fclose(fp);
        }
    }
    delete[] buffer_pool;
}

/****找到buffer中空的block位置，找不到返回-1*****/
int BufferManager::getBufferEmpty()
{
    int ret = -1;
    /*把目前内存池中经过修改的块写回文件保存*/
    for (int i = 0; i < Buffer_MAX; i++)
    {
        if (buffer_pool[i].isdirty)
        {
            FILE* fp = fopen((buffer_pool[i].file + ".data").c_str(), "rb+");
            if (fp == NULL)
            {
                fp = fopen((buffer_pool[i].file + ".data").c_str(), "wb+");
            }
            fseek(fp, buffer_pool[i].fileoffset * MAX_BLOCK_SIZE, 0);
            fwrite(buffer_pool[i].buf, MAX_BLOCK_SIZE, 1, fp);
            fclose(fp);
            buffer_pool[i].isdirty = false;
        }
    }
    /*找到新的空间，返回的是块处于BufferManager的编号*/
    for (int i = 0; i < Buffer_MAX; i++)
    {
        if (!buffer_pool[i].spaceused) //没有锁住
        {
            if (ret == -1 || buffer_pool[i].LRU_time < buffer_pool[ret].LRU_time)
            {
                ret = i;
                //buffer_pool[i].LRU_time = clock();
            }
        }
    }
    if (ret != -1)
    {
        buffer_pool[ret].LRU_time = clock();
        memset(buffer_pool[ret].buf, 0, MAX_BLOCK_SIZE);
    }
    return ret;
}

/*找到返回buffer中的空余位置，并且读取相应的文件(建立部分由)*/
int BufferManager::ReadIndexFromFile(const string& filename, int offset)
{
    for (int i = 0; i < Buffer_MAX; i++)
    {
        //已经在buffer中的块直接输出编号
        if (buffer_pool[i].file == filename && buffer_pool[i].fileoffset == offset)
            return i;
    }

    //no block have the table information
    int emptyid = getBufferEmpty(); //分配空间
    if (emptyid != -1)               //加载文件
    {
        FILE* fp = fopen((filename + ".data").c_str(), "rb+");
        if (fp == NULL)
            fp = fopen((filename + ".data").c_str(), "wb+");

        buffer_pool[emptyid].file = filename;
        buffer_pool[emptyid].fileoffset = offset;
        buffer_pool[emptyid].LRU_time = clock();

        fseek(fp, offset * MAX_BLOCK_SIZE, 0);
        fread(buffer_pool[emptyid].buf, MAX_BLOCK_SIZE, 1, fp);
        fclose(fp);
        return emptyid;
    }
    else
    {
        cout << "Buffer Memory is Full!" << endl;
        return -1;
    }
}

char* BufferManager::ReadBlockFromFile(int BlockId)
{
    buffer_pool[BlockId].LRU_time = clock();
    return buffer_pool[BlockId].buf;
}

void BufferManager::DirtyBlock(int BlockId) { buffer_pool[BlockId].isdirty = true; }
void BufferManager::PinBlock(int BlockId) { buffer_pool[BlockId].spaceused = true; }
bool BufferManager::IsDirtyBlock(int BlockId) { return buffer_pool[BlockId].isdirty; }
void BufferManager::UnPinBlock(int BlockId) { buffer_pool[BlockId].spaceused = false; }

/*将与文件有关的块全部抹除*/
void BufferManager::FreeBlock(const string& filename)
{
    for (int i = 0; i < Buffer_MAX; i++)
    {
        if (buffer_pool[i].file == filename)
        {
            buffer_pool[i].file = "";
            buffer_pool[i].fileoffset = 0;
            buffer_pool[i].isdirty = false;
            buffer_pool[i].spaceused = 0;
        }
    }
}

bool BufferManager::writetofile(const int BlockId)
{
    if (buffer_pool[BlockId].isdirty)
    {
        FILE* fp = fopen((buffer_pool[BlockId].file + ".data").c_str(), "rb+");
        if (fp == NULL)
        {
            fp = fopen((buffer_pool[BlockId].file + ".data").c_str(), "wb+");
            return false;
        }
        //cout << "正在将" << BlockId << "块写入文件位置:" << buffer_pool[BlockId].fileoffset << endl;

        fseek(fp, buffer_pool[BlockId].fileoffset * MAX_BLOCK_SIZE, 0);
        fwrite(buffer_pool[BlockId].buf, MAX_BLOCK_SIZE, 1, fp);
        fclose(fp);
        buffer_pool[BlockId].isdirty = false;
    }
    //FreeBlock(buffer_pool[BlockId].file);
    //buffer_pool[BlockId].LRU_time = 0;
    return true;
}

bool BufferManager::writetofile(Block* b)
{
    if (b->isdirty)
    {
        FILE* fp = fopen((b->file + ".data").c_str(), "rb+");
        if (fp == NULL)
        {
            fp = fopen((b->file + ".data").c_str(), "wb+");
        }
        fseek(fp, b->fileoffset * MAX_BLOCK_SIZE, 0);
        fwrite(b->buf, MAX_BLOCK_SIZE, 1, fp);
        fclose(fp);
        b->isdirty = false;
        //b->file = "";
    }
    //FreeBlock(b->file);
    //b->LRU_time = 0;
    return true;
}


char* BufferManager::FindLastIndex(int index) {//find the min index of element without using
    char* ret = NULL;
    for (int i = 0; i < 4096; i++) {//linear search  
        if (!buffer_pool[index].buf[i]) {
            ret = buffer_pool[index].buf + i; //get the address
            break;
        }
    }
    return ret;
}

Block BufferManager::FindBlock(int BlockId) {//return the BlockId'th block
    return buffer_pool[BlockId];
}

void BufferManager::UpdateBuffer_pool(Block * buffer_temp) {
    buffer_pool = buffer_temp;
}