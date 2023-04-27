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
    std::cout << "time to build NN_Graph: " << timer.stop() << std::endl;

    std::unordered_map<std::tuple<Cluster*, Cluster*>, float, Tuple_Hash> score_map;
    std::set<std::tuple<float, Cluster*, Cluster*>> sorted_cache;

    std::vector<Cluster*> to_update(cls_graph.begin(), cls_graph.end());

    while(1)
    {
        int num_cls = cls_graph.size();
        find_best_pair_parallel(to_update, cls_graph, score_map, sorted_cache);
        std::tuple<float, Cluster*, Cluster*> top = *sorted_cache.begin();
        //std::cout << "cache size: " << sorted_cache.size() << ", map size: " << score_map.size() << std::endl;
        std::cout << "number of clusters: " << cls_graph.size() << ", score improvement: " << std::get<0>(top) << std::endl;

        /*for(auto &e : score_map)
        {
            std::cout << std::get<0>(e.first)->to_string() << " - " << std::get<1>(e.first)->to_string() << ": " << e.second << std::endl;
        }*/

        // if the score is worse then stop
        if(std::get<0>(top) >= 0) break;
        
        // store old values for updated value
        Cluster *cl1 = std::get<1>(top), *cl2 = std::get<2>(top);
        
        // invalidate cache
        invalidate_cache(score_map, sorted_cache, cls_graph, cl1);
        invalidate_cache(score_map, sorted_cache, cls_graph, cl2);

        // join clusters and update the score
        Cluster *cl_joined = cls_graph.join(cl1, cl2);
        
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

void Greedy_Joining::find_best_pair_parallel(std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, std::unordered_map<std::tuple<Cluster*, Cluster*>, float, Tuple_Hash> &score_map, std::set<std::tuple<float, Cluster*, Cluster*>> &cache)
{
    int size = to_update.size();
    std::mutex best_mutex;
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; i++)
    {
        int start = i*size/num_threads;
        int end = (i+1)*size/num_threads;
        if(start == end) continue;
        threads.emplace_back(&Greedy_Joining::find_best_pair, this, std::ref(best_mutex), std::ref(to_update), std::ref(cls_graph), start, end, std::ref(score_map), std::ref(cache));
    }

    for(std::thread &thread : threads)
    {
        thread.join();
    }
}

void Greedy_Joining::find_best_pair(std::mutex &mtx, std::vector<Cluster*> &to_update, Cluster_Graph &cls_graph, int start, int end, std::unordered_map<std::tuple<Cluster*, Cluster*>, float, Tuple_Hash> &score_map, std::set<std::tuple<float, Cluster*, Cluster*>> &cache)
{
    for(int i = start; i < end; i++)
    {
        Cluster *cl1 = to_update[i];
        float cl1_size = cl1->size();
        //std::cout << i++ << std::endl;
        //std::cout << cl1->to_string() << ", children: ";
        std::vector<Cluster*> children;
        cls_graph.get_neighbours(children, cl1);
        for(Cluster *cl2 : children)
        {
            //std::cout << cl2->to_string() << ", ";
            //std::cout << "(" << i << ", " << j << ")" << std::endl;
            mtx.lock();
            std::tuple<Cluster*, Cluster*> key = get_key(cl1, cl2);
            bool seen = score_map.find(key) != score_map.end();
            mtx.unlock();
            if(seen) continue;

            float cl2_size = cl2->size();
            float f_diff = cl1_size*cl2->get_sum_of_squares() + cl2_size*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum());
            float d_diff = Util::d_all_pairs(cl1_size, distance) + Util::d_all_pairs(cl2_size, distance) - Util::d_all_pairs(cl1_size+cl2_size, distance);
            cmp_count++;

            mtx.lock();
            score_map[key] = f_diff+d_diff;
            cache.insert(std::make_tuple(f_diff+d_diff, std::get<0>(key), std::get<1>(key)));
            mtx.unlock();
        }
        //std::cout << std::endl;;
    }
}


void Greedy_Joining::invalidate_cache(std::unordered_map<std::tuple<Cluster*, Cluster*>, float, Tuple_Hash> &score_map, std::set<std::tuple<float, Cluster*, Cluster*>> &cache, Cluster_Graph &cls_graph, Cluster *cl)
{
    //std::cout << "before: cache size: " << cache.size() << ", map size: " << score_map.size() << std::endl;

    std::vector<Cluster*> children;
    cls_graph.get_children(children, cl);
    for(Cluster *next : children)
    {
        std::tuple<Cluster*, Cluster*> pair = get_key(cl, next);
        cache.erase(std::make_tuple(score_map[pair], std::get<0>(pair), std::get<1>(pair)));
        score_map.erase(pair);
    }

    //std::cout << "after: cache size: " << cache.size() << ", map size: " << score_map.size() << std::endl;
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