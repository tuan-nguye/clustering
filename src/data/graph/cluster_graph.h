#include <vector>
#include <unordered_map>

#include "data/cluster.h"
#include "data/cluster_container.h"
#include "data/graph/auto_edge_graph.h"
#include "data/graph/distance_graph.h"

#ifndef __cluster_graph_include__
#define __cluster_graph_include__

class Cluster_Graph: public Cluster_Container
{
    private:
        Auto_Edge_Graph<Cluster*> *ae_graph;
    public:
        Cluster_Graph(float d, Auto_Edge_Graph<Cluster*> *ae_graph);
        int size();

        // modifiers
        Cluster* join(Cluster *cl1, Cluster *cl2, std::vector<std::pair<Cluster*, Cluster*>> &to_update);
        // void move(); // move operation TODO

        // initialize clusters given the data
        void init_clusters_fine_grained();
        //void init_clusters_random(); // needed for other algorithms
        void delete_clusters();

        // access functions
        void get_neighbours(std::vector<Cluster*>& vec, Cluster *cl);
        bool find(Cluster *&cl);
        Cluster** begin();
        Cluster** end();
        Cluster*& operator[](int idx);
};

#endif