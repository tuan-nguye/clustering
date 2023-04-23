#include <vector>

#include "data/data.h"

#ifndef __cluster_include__
#define __cluster_include__


class Cluster: public std::vector<Data*>
{
    private:
        std::vector<float> sum;
    public:
        float sum_of_squares = 0;
        std::vector<float>& get_sum() { return sum; }
};

#endif