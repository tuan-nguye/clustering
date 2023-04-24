#include <vector>

#include "data/cluster.h"
#include "data/data.h"

#ifndef __cluster_vector_include__
#define __cluster_vector_include__

class Cluster_Vector: public std::vector<Cluster*>
{
    public:
        Cluster* add_empty_cluster();
        // join the clusters on index i and j
        // return the index of the new cluster
        virtual int join(int i, int j);
};

#endif