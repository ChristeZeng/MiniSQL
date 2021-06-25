#ifndef __H_ATTRIBUTE_H__
#define __H_ATTRIBUTE_H__

#include <string>
using namespace std;

enum AttributeType { INT, FLOAT, STRING };

class Attribute
{
public:
    string name;
    AttributeType type;
    int length;         //char�ĳ���
    bool isUnique;
    bool isNULL;
    bool isPrimary;
};

#endif