#include <cfloat>
#include <iostream>
#include <unordered_map>
#include <thread>

#include "algorithm/greedy_joining.h"
#include "util/util.h"

extern int num_threads;

std::unordered_map<Data*, std::string> Greedy_Joining::execute(std::vector<Data*> input, float dist)
{
    distance = dist;
    Cluster_Graph cls_graph(dist);
    for(Data *d : input) cls_graph.add_data(d);
    cls_graph.init_clusters_fine_grained();

    while(1)
    {
        int num_cls = cls_graph.size();
        float best_diff[2] = {1.0e38f, 1.0e38f};
        Cluster *best_pair[] = {nullptr, nullptr};
        find_best_pair_parallel(best_pair, best_diff, cls_graph);
        
        //if(best_pair[0] != nullptr) std::cout << "best pair: (" << best_pair[0]->to_string() << ", " << best_pair[1]->to_string() << ")" << std::endl;
        std::cout << "number of clusters: " << cls_graph.size() << ", score improvement: " << best_diff[0]+best_diff[1] << std::endl;

        // if the score is worse then stop
        if(best_diff[0]+best_diff[1] >= 0) break;
        
        // store old values for updated value
        Cluster *cl1 = best_pair[0], *cl2 = best_pair[1];
        
        // join clusters and update the score
        Cluster *cl_joined = cls_graph.join(best_pair[0], best_pair[1]);
    }

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

void Greedy_Joining::find_best_pair_parallel(Cluster *best_pair[2], float best_diff[2], Cluster_Graph &cls_graph)
{
    int cls_size = cls_graph.size();
    std::mutex best_mutex;
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; i++)
    {
        int start = i*cls_size/num_threads;
        int end = (i+1)*cls_size/num_threads;
        if(start == end) continue;
        threads.emplace_back(&Greedy_Joining::find_best_pair, this, best_pair, best_diff, std::ref(best_mutex), std::ref(cls_graph), start, end);
    }

    for(std::thread &thread : threads)
    {
        thread.join();
    }
}

void Greedy_Joining::find_best_pair(Cluster *best_pair[2], float best_diff[2], std::mutex &best_mutex, Cluster_Graph &cls_graph, int start, int end)
{
    Cluster *local_best[2] = {nullptr, nullptr};
    float local_best_diff[2] = {1.0e38f, 1.0e38f};

    for(int i = start; i < end; i++)
    {
        Cluster *cl1 = cls_graph[i];
        float cl1_size = cl1->size();
        //std::cout << i++ << std::endl;
        //std::cout << cl1->to_string() << ", children: ";
        std::vector<Cluster*> children;
        cls_graph.get_neighbours(children, cl1);
        for(Cluster *cl2 : children)
        {
            //std::cout << cl2->to_string() << ", ";
            //std::cout << "(" << i << ", " << j << ")" << std::endl;
            float cl2_size = cl2->size();
            float f_diff = cl1_size*cl2->get_sum_of_squares() + cl2_size*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum());
            float d_diff = Util::d_all_pairs(cl1_size, distance) + Util::d_all_pairs(cl2_size, distance) - Util::d_all_pairs(cl1_size+cl2_size, distance);
            
            if(f_diff+d_diff < local_best_diff[0]+local_best_diff[1])
            {
                local_best_diff[0] = f_diff;
                local_best_diff[1] = d_diff;
                local_best[0] = cl1;
                local_best[1] = cl2;
            }
        }
        //std::cout << std::endl;;
    }

    std::lock_guard<std::mutex> lock(best_mutex);
    if(local_best_diff[0]+local_best_diff[1] < best_diff[0]+best_diff[1])
    {
        best_pair[0] = local_best[0];
        best_pair[1] = local_best[1];
        best_diff[0] = local_best_diff[0];
        best_diff[1] = local_best_diff[1];
    }
}