#pragma once
#include <string>
#include <ctime>
using namespace std;

//const int MAX_BLOCK_SIZE = 4096;
//#define MAX_BLOCK_SIZE = 4096


class Block
{
public:
    char buf[4096];    //һ��Block 4KB�Ĵ�С
    string file;       //�ļ���
    clock_t LRU_time;  //LRU���  
    int fileoffset;    //һ���ļ�����飬ÿ��������λ�ü�Ϊblock offset
    bool spaceused;    //pin�ı��
    bool isdirty;      //�Ƿ��޸Ĺ��ı��
};
