#ifndef __H_TUPLE_H__
#define __H_TUPLE_H__

#include <vector>
#include<any>
#include<boost/variant.hpp>

using namespace std;
typedef boost::variant<int,float,string> value;


class Tuple
{
public:
    vector<value>v;
};

#endif