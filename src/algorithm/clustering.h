#include <map>
#include <iostream>

#include "data/cluster.h"
#include "data/cluster_vector.h"

#ifndef __clustering_include__
#define __clustering_include__

class Clustering
{
    protected:
        Cluster_Vector cls;
        std::map<Cluster, double> f;
        double d(int cl_size, double d)
        {
            double res = cl_size*(cl_size-1)/2 * d*d;
            //std::cout << "d: " << res << std::endl;
            return res;
        }

        void reset_state()
        {
            cls.clear();
            f.clear();
        }
    public:
        virtual std::map<Data*, int> execute(std::vector<Data*> input, double d) = 0;
};

#endif