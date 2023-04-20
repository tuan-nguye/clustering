
#include "algorithm/clustering.h"

#ifndef __greedy_joining_include__
#define __greedy_joining_include__

class Greedy_Joining: public Clustering
{
    private:
    public:
        std::unordered_map<Data*, std::string> execute(std::vector<Data*> input, float d);
};

#endif