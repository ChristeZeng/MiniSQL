#pragma once
#include <string>
#include <ctime>
using namespace std;

//const int MAX_BLOCK_SIZE = 4096;
//#define MAX_BLOCK_SIZE = 4096


class Block
{
public:
    char buf[4096];    //一个Block 4KB的大小
    string file;       //文件名
    clock_t LRU_time;  //LRU标记  
    int fileoffset;    //一个文件多个块，每个块所处位置即为block offset
    bool spaceused;    //pin的标记
    bool isdirty;      //是否修改过的标记
};
