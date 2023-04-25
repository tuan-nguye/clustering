#include <mutex>

#include "algorithm/clustering.h"
#include "data/graph/cluster_graph.h"

#ifndef __greedy_joining_include__
#define __greedy_joining_include__

class Greedy_Joining: public Clustering
{
    private:
        float distance;
        void find_best_pair_parallel(Cluster *best_pair[2], float best_diff[2], Cluster_Graph &cls_graph);
        void find_best_pair(Cluster *best_pair[2], float best_diff[2], std::mutex &best_mutex, Cluster_Graph &cls_graph, int start, int end);
    public:
        std::unordered_map<Data*, std::string> execute(std::vector<Data*> input, float d);
};

#endif