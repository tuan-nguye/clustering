#include <mutex>
#include <unordered_map>
#include <set>
#include <tuple>
#include <queue>

#include "algorithm/clustering.h"
#include "data/cluster_container.h"
#include "data/cluster_vector.h"
#include "data/graph/cluster_graph.h"
#include "data/structures/queue_defs.h"
#include "data/structures/union_find.h"

#ifndef __greedy_joining_include__
#define __greedy_joining_include__

class Greedy_Joining: public Clustering
{
    private:
        // optimization configuration
        
        // enables cache, tradeoff space <-> time
        bool cache_enabled = false;

        // selects container type, default is vector
        // graphs are the performance options
        Cluster_Container *cls_container;

        // other variables
        float distance;

        struct Cache
        {
            MinPriorityQueue pq;
            std::unordered_set<Cluster*> invalid;
            std::vector<Cluster*> to_init;
            std::vector<std::pair<Cluster*, Cluster*>> to_update;
        };


        // functions updating cache with new values
        // init_cache for initializing the cache with values when its empty
        void init_cache(Cache &cache, Cluster_Container *cls_container);
        void init_cache_single(Cache &cache, Cluster_Container *cls_container);
        void init_cache_parallel(Cache &cache, Cluster_Container *cls_container);
        void init_cache_parallel_thread(Cache &cache, Cluster_Container *cls_container, std::mutex *mtx, int start, int end);
        void init_cache_operation(Cache &cache, Cluster_Container *cls_container, std::mutex *mtx, Cluster *cl1);
        // update cache after joining two nodes
        void update_cache(Cache &cache, Cluster_Container *cls_container);

        // functions for iterating through all pairs
        void best_pair_iterate(Edge &best, Cluster_Container *cls_container);
        void best_pair_iterate_single(Edge &best, Cluster_Container *cls_container);
        void best_pair_iterate_parallel(Edge &best, Cluster_Container *cls_container);
        void best_pair_iterate_parallel_thread(Edge &best, Cluster_Container *cls_container, std::mutex *mtx, int start, int end);
        void best_pair_iterate_operation(Edge &best, Cluster_Container *cls_container, std::mutex *mtx, Cluster *cl1);

        // get next valid pair of clusters to join
        Edge get_next_pair_pq(Cache &cache);
        Edge get_next_pair_iterate(Cluster_Container *cls_container);
        void join_clusters(Edge &e, Cache &cache, Cluster_Container *cls_container, Union_Find<Cluster*> &uf);
    public:
        Greedy_Joining() {};
        std::unordered_map<Data*, std::string> execute(std::vector<Data*> &input, float d);

        void set_cache(bool value) { cache_enabled = value; }
        void set_container(Cluster_Container *cls_cont) { cls_container = cls_cont; }
};

#endif