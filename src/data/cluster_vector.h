
#include "data/cluster_container.h"
#include "data/structures/maptor.h"

#ifndef __cluster_vector_include__
#define __cluster_vector_include__

class Cluster_Vector: public Cluster_Container
{
    private:
        Maptor<Cluster*> clusters;
    public:
        Cluster_Vector(float d): Cluster_Container(d) {}
        int size();

        // modifiers
        Cluster* join(Cluster *cl1, Cluster *cl2);
        // void move(); // move operation TODO

        // initialize clusters given the data
        void init_clusters_fine_grained();
        //void init_clusters_random(); // needed for other algorithms
        
        // access functions
        void get_neighbours(std::vector<Cluster*>& vec, Cluster *cl);
        bool find(Cluster *&cl);
        Cluster** begin();
        Cluster** end();
        Cluster*& operator[](int idx);
};

#endif