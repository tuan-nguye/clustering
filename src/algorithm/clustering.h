#include <unordered_map>
#include <iostream>
#include <string>

#include "data/cluster.h"
#include "data/cluster_vector.h"
#include "data/graph/cluster_graph.h"

#ifndef __clustering_include__
#define __clustering_include__

class Clustering
{
    protected:
        Cluster_Vector cls;
        std::unordered_map<Cluster*, float> f;
        float d(int cl_size, float d)
        {
            float res = (cl_size*(cl_size-1))/2 * d*d;
            //std::cout << "d: " << res << std::endl;
            return res;
        }

        void reset_state()
        {
            cls.clear();
            f.clear();
        }
    public:
        virtual std::unordered_map<Data*, std::string> execute(std::vector<Data*> input, float d) = 0;
};

#endif