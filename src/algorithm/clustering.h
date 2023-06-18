#include <unordered_map>
#include <iostream>
#include <string>

#include "data/cluster.h"
#include "data/graph/cluster_graph.h"

#ifndef __clustering_include__
#define __clustering_include__

class Clustering
{
    public:
        virtual std::unordered_map<Data*, std::string> execute(std::vector<Data*> input, float d) = 0;
        std::string generate_label(int n)
        {
            std::string label;

            while(n >= 0)
            {
                label = char('a' + n%26) + label;
                n = n/26 - 1;
            }

            return label;
        }
};

#endif