#ifndef __H_CONDITION_H__
#define __H_CONDITION_H__

#include <string>
#include <boost/variant.hpp>

using namespace std;

enum  ConditionType { EQ, NE, LT, LE, GT, GE };
//using value = boost::variant<int, float, string>;
typedef boost::variant<int, float, string> value;

class Condition
{
public:
    string attri;
    //value val;
    value val;
    ConditionType type;
};

#endif