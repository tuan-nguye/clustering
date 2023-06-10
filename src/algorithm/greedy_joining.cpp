#include <iostream>
#include <cfloat>
#include <set>
#include <thread>
#include <stdexcept>

#include "algorithm/greedy_joining.h"
#include "util/util.h"
#include "util/time/time.h"

extern int num_threads;

std::unordered_map<Data*, std::string> Greedy_Joining::execute(std::vector<Data*> input, float dist)
{
    cmp_count = 0;
    Time timer;
    timer.start();
    distance = dist;
    Cluster_Graph cls_graph(dist);
    for(Data *d : input) cls_graph.add_data(d);
    cls_graph.init_clusters_fine_grained();
    std::cout << "time to build Distance_Graph: " << timer.stop() << std::endl;
    std::unordered_set<uintptr_t> addresses;
    for(Cluster *cl : cls_graph) addresses.insert(reinterpret_cast<uintptr_t>(cl));
    
    MinPriorityQueue pq;
    std::unordered_set<Cluster*> invalid;

    std::vector<Cluster*> to_update(cls_graph.begin(), cls_graph.end());

    while(1)
    {
        int num_cls = cls_graph.size();
        update_clusters(to_update, cls_graph, pq);
        Edge top = get_next_pair(pq, invalid);

        //std::cout << "cl1 exists: " << cls_graph.find(std::get<1>(top)) << ", cl2 exists: " << cls_graph.find(std::get<2>(top)) << std::endl;
        std::cout << "number of clusters: " << cls_graph.size() << ", pq size: " << pq.size() << ", score improvement: " << std::get<0>(top) << std::endl;

        //for(Cluster *cl : cls_graph) std::cout << cl->to_string() << std::endl;

        // if the score is worse then stop
        if(std::get<0>(top) >= 0) break;

        //std::cout << "join: " << std::get<1>(top)->to_string() << " - " << std::get<2>(top)->to_string() << std::endl;
        Cluster *cl1 = std::get<1>(top), *cl2 = std::get<2>(top);

        // join clusters and update the score
        //std::cout << "num clusters before: " << cls_graph.size() << std::endl;
        Cluster *cl_joined = cls_graph.join(cl1, cl2);
        invalid.insert(cl1);
        invalid.insert(cl2);
        //std::cout << "cl_joined pointer reused: " << (addresses.find(reinterpret_cast<uintptr_t>(cl_joined)) != addresses.end()) << std::endl;
        //std::cout << "num clusters after: " << cls_graph.size() << std::endl;
        
        // select next clusters to update cache
        to_update.clear();
        to_update.push_back(cl_joined);
    }

    std::cout << "cmp_count: " << cmp_count << std::endl;

    std::unordered_map<Data*, std::string> cluster_map;
    int label = 0;
    for(auto &cl : cls_graph)
    {
        for(auto &elem : *cl)
        {
            cluster_map[elem] = std::to_string(label);
        }
        label++;
    }

    return cluster_map;
}

void Greedy_Joining::update_clusters(std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, MinPriorityQueue &pq)
{
    if(to_update.size() == 1)
    {
        update_clusters_single(to_update[0], cls_graph, pq);
    } else
    {
        update_clusters_parallel(to_update, cls_graph, pq);
    }
}

void Greedy_Joining::update_clusters_parallel(std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, MinPriorityQueue &pq)
{
    int size = to_update.size();
    std::mutex mtx;
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; i++)
    {
        int start = i*size/num_threads;
        int end = (i+1)*size/num_threads;
        if(start == end) continue;
        threads.emplace_back(&Greedy_Joining::update_clusters_parallel_thread, this, std::ref(mtx), std::ref(to_update), std::ref(cls_graph), start, end, std::ref(pq));
    }

    for(std::thread &thread : threads)
    {
        thread.join();
    }
}

void Greedy_Joining::update_clusters_parallel_thread(std::mutex &mtx, std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, int start, int end, MinPriorityQueue &pq)
{
    for(int i = start; i < end; i++)
    {
        Cluster *cl1 = to_update[i];
        float cl1_size = cl1->size();
        //std::cout << i++ << std::endl;
        std::vector<Cluster*> children;
        cls_graph.get_neighbours(children, cl1);
        //std::string out = "num of children: " + std::to_string(children.size()) + "\n";
        //out += cl1->to_string() + ", children: ";
        for(Cluster *cl2 : children)
        {
            //out += cl2->to_string() + ", ";

            float cl2_size = cl2->size();
            float f_diff = cl1_size*cl2->get_sum_of_squares() + cl2_size*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum());
            float d_diff = Util::d_all_pairs(cl1_size, distance) + Util::d_all_pairs(cl2_size, distance) - Util::d_all_pairs(cl1_size+cl2_size, distance);
            cmp_count++;

            if(f_diff+d_diff < 0)
            {
                mtx.lock();
                pq.emplace(f_diff+d_diff, cl1, cl2);
                mtx.unlock();
            }
        }
        //std::cout << out << std::endl;
    }
}

void Greedy_Joining::update_clusters_single(Cluster *cl1, Cluster_Graph &cls_graph, MinPriorityQueue &pq)
{
    float cl1_size = cl1->size();
    //std::cout << i++ << std::endl;
    std::vector<Cluster*> children;
    cls_graph.get_neighbours(children, cl1);
    //std::cout << "num of children: " << children.size() << std::endl;
    //std::cout << cl1->to_string() << ", children: ";
    for(Cluster *cl2 : children)
    {
        //std::cout << cl2->to_string() << ", ";
        std::tuple<Cluster*, Cluster*> key = get_key(cl1, cl2);

        float cl2_size = cl2->size();
        float f_diff = cl1_size*cl2->get_sum_of_squares() + cl2_size*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum());
        float d_diff = Util::d_all_pairs(cl1_size, distance) + Util::d_all_pairs(cl2_size, distance) - Util::d_all_pairs(cl1_size+cl2_size, distance);
        cmp_count++;

        if(f_diff+d_diff < 0) pq.emplace(f_diff+d_diff, cl1, cl2);
    }
    //std::cout << std::endl;
}

Edge Greedy_Joining::get_next_pair(MinPriorityQueue &pq, std::unordered_set<Cluster*> invalid)
{
    Edge top;

    do
    {
        if(pq.empty()) return std::make_tuple(0, nullptr, nullptr);
        top = pq.top();
        pq.pop();
    } while(invalid.count(std::get<1>(top)) == 1 || invalid.count(std::get<2>(top)) == 1);

    return top;
}


std::tuple<Cluster*, Cluster*> Greedy_Joining::get_key(Cluster *cl1, Cluster *cl2)
{
    if(cl1 == cl2) {
        throw std::invalid_argument("get_key: cl1 == cl2");
    } else if(cl1 > cl2)
    {
        Cluster *temp = cl1;
        cl1 = cl2;
        cl2 = temp;
    }

    return std::make_tuple(cl1, cl2);
}