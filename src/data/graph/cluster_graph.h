#include <vector>
#include <unordered_map>

#include "data/cluster.h"
#include "data/graph/nearest_neighbour_graph.h"

#ifndef __cluster_graph_include__
#define __cluster_graph_include__

class Cluster_Graph: public NN_Graph<Cluster*>
{
    private:
        float d;
        std::vector<Data*> data;
        
    public:
        Cluster_Graph(float d);
        void add_data(Data *d);
        int size();

        // modifiers
        Cluster* join(Cluster *cl1, Cluster *cl2);
        // void move(); // move operation TODO

        // initialize clusters given the data
        void init_clusters_fine_grained();
        //void init_clusters_random(); // needed for other algorithms
        
        // access functions
        void get_neighbours(std::vector<Cluster*>& vec, Cluster *cl);
        Cluster** begin();
        Cluster** end();
        Cluster*& operator[](int idx);

        // comparator for nearest neighbour graph
        static float compare(Cluster *&cl1, Cluster *&cl2);
};

#endif