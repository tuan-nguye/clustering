
#include "data/cluster_container.h"
#include "data/structures/maptor.h"

#ifndef __cluster_vector_include__
#define __cluster_vector_include__


/**
 * @brief cluster container using a maptor for fast iteration and unique objects.
 * 
 */
class Cluster_Vector: public Cluster_Container
{
    private:
        // stores all clusters
        Maptor<Cluster*> clusters;
    public:
        /**
         * @brief Construct a new Cluster_Vector object
         * 
         * @param d distance threshold
         */
        Cluster_Vector(float d): Cluster_Container(d) {}
        // return the number of clusters
        int size();

        //// modifiers

        // join to clusters cl1 and cl2 and return the new joined cluster
        Cluster* join(Cluster *cl1, Cluster *cl2, std::vector<std::pair<Cluster*, Cluster*>> &to_update);
        // void move(); // move operation TODO

        // initialize clusters given the data
        void init_clusters_fine_grained();
        //void init_clusters_random(); // needed for other algorithms

        // Rebuilding is not supported for this container
        void rebuild(std::vector<std::pair<Cluster*, Cluster*>> &to_update) {}

        //// access functions

        // get neighbours for a cluster cl and store them in vec
        void get_neighbours(std::vector<Cluster*>& vec, Cluster *cl);
        // returns true if a cluster is contained
        bool find(Cluster *&cl);
        // returns a pointer to the first cluster object. can be used as iterator
        Cluster** begin();
        // returns a pointer behind the last cluster object
        Cluster** end();
        // access operator
        Cluster*& operator[](int idx);
        // clear all clusters and elements
        void clear();
};

#endif