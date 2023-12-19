#include <vector>
#include <unordered_map>

#include "data/cluster.h"
#include "data/cluster_container.h"
#include "data/graph/auto_edge_graph.h"
#include "data/graph/distance_graph.h"

#ifndef __cluster_graph_include__
#define __cluster_graph_include__

/**
 * @brief Graph Container for clusters which features joining two
 * for the greedy joining algorithm and access to all its neighbours
 * to iterate through to find the best pair of clusters.
 * 
 */
class Cluster_Graph: public Cluster_Container
{
    private:
        Auto_Edge_Graph<Cluster*> *ae_graph;
    public:
        /**
         * @brief Construct a new Cluster_Graph object, by providing a constant d
         * needed for the cost function and an underlying ae_graph.
         * 
         * @param d 
         * @param ae_graph 
         */
        Cluster_Graph(float d, Auto_Edge_Graph<Cluster*> *ae_graph);
        /**
         * @brief returns the number of clusters currently in the graph
         * 
         * @return int 
         */
        int size();

        // modifiers
        /**
         * @brief join two clusters and update its nodes and edges
         * 
         * @param cl1 
         * @param cl2 
         * @param to_update 
         * @return Cluster* new cluster object which resulted from the join operation
         */
        Cluster* join(Cluster *cl1, Cluster *cl2, std::vector<std::pair<Cluster*, Cluster*>> &to_update);
        // void move(); // move operation TODO

        // initialize clusters given the data
        /**
         * @brief initialize all clusters by giving each element its own cluster
         * 
         */
        void init_clusters_fine_grained();
        //void init_clusters_random(); // needed for other algorithms
        void rebuild(std::vector<std::pair<Cluster*, Cluster*>> &to_update);

        // access functions
        void get_neighbours(std::vector<Cluster*>& vec, Cluster *cl);
        bool find(Cluster *&cl);
        Cluster** begin();
        Cluster** end();
        Cluster*& operator[](int idx);

        void clear();
};

#endif