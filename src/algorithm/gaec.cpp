#include <iostream>
#include <vector>
#include <thread>

#include "algorithm/gaec.h"
#include "andres/graph/multicut/greedy-additive.hxx"
#include "util/util_math.h"
#include "data/structures/union_find.h"

extern int num_threads;

/**
 * @brief greedy additive edge contraction algorithm
 * First create a complete graph and add a weight which indicates the distance between
 * each cluster. Then use the gaec implementation from the included library. To join the clusters
 * a union-find structure is used.
 * @param input the input data
 * @param d the distance threshold needed for evaluating the cost between clusters when joining
 * @return std::unordered_map<Data*, std::string> which is a map that assigns each data point a string label
 */
std::unordered_map<Data*, std::string> Greedy_Additive_EC::execute(std::vector<Data*> &input, float d)
{
    std::cout << "calculating edge weights..." << std::endl;
    andres::graph::Graph<> graph;
    size_t input_size = input.size();
    std::vector<float> weights;
    int idx_w = 0;
    graph.insertVertices(input_size);
    add_weighted_edge(input, graph, weights);

    // disjoint set structure for clustering
    Union_Find<Data*> uf(input_size);
    for(int i = 0; i < input_size; i++) uf.insert(input[i]);

    std::vector<char> edge_labels(weights.size());

    std::cout << "executing gaec..." << std::endl;
    andres::graph::multicut::greedyAdditiveEdgeContraction(graph, weights, edge_labels);

    float score = 0.0f;
    // iterate through labels and edges and join elements that appear in the same cluster
    for(int i = 0; i < graph.numberOfEdges(); i++)
    {
        int v1 = graph.vertexOfEdge(i, 0), v2 = graph.vertexOfEdge(i, 1);
        if(edge_labels[i] == 0)
        {
            uf.union_(input[v1], input[v2]);
            score += weights[i];
        }
    }

    this->set_objective_value(-score);

    // write result
    int label = 0;
    std::unordered_map<Data*, int> root_to_label;
    std::unordered_map<Data*, std::string> result;

    for(Data *d : uf)
    {
        Data *r = uf.find_(d);
        if(root_to_label.count(r) == 0) root_to_label[r] = label++;
        result[d] = this->generate_label(root_to_label[r]);
    }

    return result;
}

/**
 * @brief calculate the weights for each edge and store them in a std::vector
 * The function creates multiple threads for parallelization if it was enabled.
 * @param input the input data, used to calculate the weights
 * @param graph the graph containing the nodes and edges
 * @param weights output vector
 */
void Greedy_Additive_EC::add_weighted_edge(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights)
{
    if(!this->parallel_enabled()) add_weighted_edge_single(input, graph, weights);
    else add_weighted_edge_parallel(input, graph, weights);
}

void Greedy_Additive_EC::add_weighted_edge_single(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights)
{
    std::mutex mtx;
    for(int i = 0; i < input.size(); i++) add_weighted_edge_operation(input, graph, weights, i, mtx);
}

void Greedy_Additive_EC::add_weighted_edge_parallel(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights)
{
    std::mutex mtx;
    std::atomic<size_t> counter = 0;
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(&Greedy_Additive_EC::add_weighted_edge_thread, this, std::ref(input), std::ref(graph), std::ref(weights), std::ref(counter), std::ref(mtx));
    }

    for(std::thread &thread : threads) thread.join();
}

void Greedy_Additive_EC::add_weighted_edge_thread(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights, std::atomic<size_t> &counter, std::mutex &mtx)
{
    int input_size = input.size();
    int i;

    while((i = counter.fetch_add(1)) < input_size)
    {
        add_weighted_edge_operation(input, graph, weights, i, mtx);
    }
}

/**
 * @brief function where the weights are actually calculated and stored in the vector
 * since multiple threads can access the output vector, it needs to be synchronized with a mutex.
 * For an index i it iterates through all the data points following i in the input vector. It doesn't
 * start at i = 0 to save time because symmetric edges are not added. That means each edge is only added
 * once and not twice.
 * @param input 
 * @param graph 
 * @param weights 
 * @param i index of the current data element
 * @param mtx mutex for synchronization
 */
void Greedy_Additive_EC::add_weighted_edge_operation(std::vector<Data*> &input, andres::graph::Graph<> &graph, std::vector<float> &weights, int i, std::mutex &mtx)
{
    for(int j = i+1; j < input.size(); j++)
    {
        float weight = -eval(input[i], input[j]);
        std::lock_guard<std::mutex> lock(mtx);
        graph.insertEdge(i, j);
        weights.push_back(weight);
    }
}