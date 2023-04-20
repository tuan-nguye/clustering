
#include "algorithm/clustering.h"

#ifndef __greedy_joining_include__
#define __greedy_joining_include__

class Greedy_Joining: public Clustering
{
    private:
    public:
        std::map<Data*, int> execute(std::vector<Data*> input, double d);
};

#endif