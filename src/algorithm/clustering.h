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
};

#endif