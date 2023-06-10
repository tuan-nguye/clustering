#include <mutex>
#include <unordered_map>
#include <set>
#include <tuple>
#include <queue>

#include "algorithm/clustering.h"
#include "data/graph/cluster_graph.h"
#include "data/graph/auto_edge_graph.h"

#ifndef __greedy_joining_include__
#define __greedy_joining_include__

typedef std::tuple<float, Cluster*, Cluster*> Edge;
typedef std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> MinPriorityQueue;

class Greedy_Joining: public Clustering
{
    private:
        // optimization configuration
        
        // enables cache, tradeoff space <-> time
        bool cache = false;

        // use of graph instead of iterating through
        // all pairs of clusters
        bool graph = false;
        Auto_Edge_Graph<Cluster*> *opt_graph;

        // use parallelization if possible
        bool parallel = false;

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

        void update_clusters(std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, MinPriorityQueue &pq);
        void update_clusters_parallel(std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, MinPriorityQueue &pq);
        void update_clusters_parallel_thread(std::mutex *mtx, std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, int start, int end, MinPriorityQueue &pq);
        void update_clusters_single(std::mutex *mtx, Cluster *cl, Cluster_Graph &cls_graph, MinPriorityQueue &pq);

        Edge get_next_pair(MinPriorityQueue &pq, std::unordered_set<Cluster*> invalid);
        std::tuple<Cluster*, Cluster*> get_key(Cluster *cl1, Cluster *cl2);
    public:
        Greedy_Joining() {};
        std::unordered_map<Data*, std::string> execute(std::vector<Data*> input, float d);

        void set_cache(bool value) { cache = value; }
        void set_graph(bool value) { graph = value; }
        void set_parallel(bool value) { parallel = value; }
};

#endif