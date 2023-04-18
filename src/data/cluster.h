#include <vector>

#ifndef __cluster_include__
#define __cluster_include__

class Data;

class Cluster
{
    public:
        std::vector<Data*> elements;
        long sum = 0;
        long square_sum = 0;
};

#endif