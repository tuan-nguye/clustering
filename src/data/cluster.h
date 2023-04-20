#include <vector>

#include "data/data.h"

#ifndef __cluster_include__
#define __cluster_include__


class Cluster: public std::vector<Data*>
{
    public:
        std::vector<Data*> elements;
        std::vector<float> sum;
        double sum_of_squares = 0;
};

#endif