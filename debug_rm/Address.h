#ifndef __H_ADDRESS_H__
#define __H_ADDRESS_H__

#include <string>
using namespace std;

class Address
{
public:
    string file;
    int fileoffset;
    int blockoffset;
};

#endif