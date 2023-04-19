#include <vector>

class Cluster;
class Data;

#ifndef __cluster_vector_include__
#define __cluster_vector_include__

class Cluster_Vector
{
    public:
        std::vector<Cluster*> cluster;
        void add_data(Data *data);
        void join(int i, int j);
};

#endif