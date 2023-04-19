#include <vector>

#include "data/data.h"

#ifndef __cluster_include__
#define __cluster_include__


class Cluster
{
    private:
    public:
        std::vector<Data*> elements;
        long sum = 0;
        long square_sum = 0;
        
        void add_element(Data *d)
        {
            elements.push_back(d);
        }

        void add_all_elements(std::vector<Data*> ele)
        {
            elements.insert(elements.end(), ele.begin(), ele.end());
        }
};

#endif