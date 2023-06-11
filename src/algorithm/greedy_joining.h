#include <mutex>
#include <unordered_map>
#include <set>
#include <tuple>
#include <queue>

#include "algorithm/clustering.h"
#include "data/cluster_container.h"
#include "data/cluster_vector.h"
#include "data/graph/cluster_graph.h"

#ifndef __greedy_joining_include__
#define __greedy_joining_include__

typedef std::tuple<float, Cluster*, Cluster*> Edge;
typedef std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> MinPriorityQueue;

class Greedy_Joining: public Clustering
{
    public:
        enum container_type { VECTOR = 0, DISTANCE = 1, KNN = 2 };
    private:
        // optimization configuration
        
        // enables cache, tradeoff space <-> time
        bool cache_enabled = false;

        // selects container type, default is vector
        // graphs are the performance options
        container_type container = container_type::VECTOR;
        /* should actually be a map that stores type as key and
        the class as value to invoke the class's constructor inside
        the execution function but no idead how it works
        std::unordered_map<container_type, Cluster_Container> container_type_map =
        {
            { container_type::VECTOR, Cluster_Vector()},
        };*/

        // use parallelization if possible
        bool parallel_enabled = false;

        // other variables
        float distance;
        int cmp_count;

        struct Tuple_Hash
        {
            std::size_t operator()(const std::tuple<Cluster*, Cluster*> &t) const
            {
                return std::hash<Cluster*>{}(std::get<0>(t)) ^ std::hash<Cluster*>{}(std::get<1>(t));
            }
        };

        struct Tuple_Equal
        {
            bool operator()(const std::tuple<Cluster*, Cluster*> &t1, const std::tuple<Cluster*, Cluster*> &t2)
            {
                Cluster *cl11 = std::get<0>(t1), *cl12 = std::get<1>(t1);
                Cluster *cl21 = std::get<0>(t2), *cl22 = std::get<1>(t2);
                return (cl11 == cl21 && cl12 == cl22) || (cl11 == cl22 && cl12 == cl21);
            }
        };

        struct Cache
        {
            MinPriorityQueue pq;
            std::unordered_set<Cluster*> invalid;
            std::vector<Cluster*> to_update;
        };


        // functions updating cache with new values
        void update_cache(Cache &cache, Cluster_Container *cls_container);
        void update_cache_single(Cache &cache, Cluster_Container *cls_container);
        void update_cache_parallel(Cache &cache, Cluster_Container *cls_container);
        void update_cache_parallel_thread(Cache &cache, Cluster_Container *cls_container, std::mutex *mtx, int start, int end);
        void update_cache_operation(Cache &cache, Cluster_Container *cls_container, std::mutex *mtx, Cluster *cl1);

        // functions for iterating through all pairs
        void best_pair_iterate(Edge &best, Cluster_Container *cls_container);
        void best_pair_iterate_single(Edge &best, Cluster_Container *cls_container);
        void best_pair_iterate_parallel(Edge &best, Cluster_Container *cls_container);
        void best_pair_iterate_parallel_thread(Edge &best, Cluster_Container *cls_container, std::mutex *mtx, int start, int end);
        void best_pair_iterate_operation(Edge &best, Cluster_Container *cls_container, std::mutex *mtx, Cluster *cl1);

        // get next valid pair of clusters to join
        Edge get_next_pair_pq(Cache &cache);
        Edge get_next_pair_iterate(Cluster_Container *cls_container);
        void join_clusters(Edge &e, Cache &cache, Cluster_Container *cls_container);
    public:
        Greedy_Joining() {};
        std::unordered_map<Data*, std::string> execute(std::vector<Data*> input, float d);

        void set_cache(bool value) { cache_enabled = value; }
        void set_container(container_type type) { container = type; }
        void set_parallel(bool value) { parallel_enabled = value; }
};

#endif