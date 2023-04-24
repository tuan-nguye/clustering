#include <vector>
#include <unordered_map>

#include "data/cluster_vector.h"
#include "data/graph/graph.h"

#ifndef __cluster_graph_include__
#define __cluster_graph_include__

class Cluster_Graph: public Cluster_Vector
{
    private:
        float d;
        Graph<Cluster*> graph;
        std::unordered_map<Cluster*, int> cls_idx;
    public:
        Cluster_Graph(std::vector<Data*> data, float d);
        int join(int i, int j) override;
        void get_neighbours(std::vector<Cluster*>& vec, Cluster *cl);
        int get_cluster_index(Cluster *cl);
};

#endif