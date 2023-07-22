#include <atomic>
#include <mutex>
#include <functional>

#include "algorithm/clustering.h"
#include "andres/graph/graph.hxx"

#ifndef __gaec_include__
#define __gaec_include__

class Greedy_Additive_EC: public Clustering
{
    private:
        std::function<float(Data*, Data*)> eval;

        void add_weighted_edge(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights);
        void add_weighted_edge_single(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights);
        void add_weighted_edge_parallel(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights);
        void add_weighted_edge_thread(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights, std::atomic<size_t> &counter, std::mutex &mtx);
        void add_weighted_edge_operation(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights, int idx, std::mutex &mtx);
    public:
        Greedy_Additive_EC(std::function<float(Data*, Data*)> eval): eval(eval) {}
        std::unordered_map<Data*, std::string> execute(std::vector<Data*> &input, float d);
};

#endif