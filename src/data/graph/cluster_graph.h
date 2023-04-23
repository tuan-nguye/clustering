#include <vector>
#include <unordered_map>

#include "data/cluster_vector.h"
#include "data/graph/graph.h"

#ifndef __cluster_graph_include__
#define __cluter_graph_include__

class Cluster_Graph: public Cluster_Vector
{
    private:
        float d;
        Graph<Cluster*> graph;
    public:
        Cluster_Graph(std::vector<Data*> data, float d);
        int join(int i, int j) override;
};

#endif