#include <vector>

#include "data/cluster.h"
#include "data/data.h"

#ifndef __cluster_vector_include__
#define __cluster_vector_include__

class Cluster_Vector
{
    public:
        std::vector<Cluster> clusters;
        void add_data(Data *data);
        // join the clusters on index i and j
        // return the index of the new cluster
        int join(int i, int j);
};

#endif