#include <vector>

#include "data/data.h"
#include "data/cluster.h"

#ifndef __cluster_container_include__
#define __cluster_container_include__

class Cluster_Container
{
    private:
        float d;
        std::vector<Data*> data;
    public:
        Cluster_Container(float d): d(d) {}
        void add_data(Data *d) { data.push_back(d); }
        virtual int size() = 0;

        // modifiers
        virtual Cluster* join(Cluster *cl1, Cluster *cl2, std::vector<std::pair<Cluster*, Cluster*>> &to_update) = 0;
        // void move(); // move operation TODO

        // initialize clusters given the data
        virtual void init_clusters_fine_grained() = 0;
        //void init_clusters_random(); // needed for other algorithms
        virtual void rebuild(std::vector<std::pair<Cluster*, Cluster*>> &to_update) = 0;    // rebuild could yield new edges, for lazy data structures
        
        // delete clusters from heap
        virtual void delete_clusters() = 0;

        // access functions
        virtual void get_neighbours(std::vector<Cluster*>& vec, Cluster *cl) = 0;
        virtual bool find(Cluster *&cl) = 0;
        virtual Cluster** begin() = 0;
        virtual Cluster** end() = 0;
        virtual Cluster*& operator[](int idx) = 0;

        float get_d() { return d; }
        std::vector<Data*>& get_data() { return data; }
};

#endif