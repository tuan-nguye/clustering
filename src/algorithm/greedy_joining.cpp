#include <iostream>
#include <cfloat>
#include <set>
#include <thread>
#include <stdexcept>

#include "algorithm/greedy_joining.h"
#include "util/util_math.h"
#include "util/util_cluster.h"
#include "util/time/time.h"

extern int num_threads;

std::unordered_map<Data*, std::string> Greedy_Joining::execute(std::vector<Data*> input, float dist)
{
    double score = 0;
    cmp_count = 0;
    Time timer;
    timer.start();
    distance = dist;
    
    // clear cls_container
    for(Data *d : input) cls_container->add_data(d);
    cls_container->init_clusters_fine_grained();
    (*cls_container)[0]->to_string();
    std::cout << "time to build container structure: " << timer.stop() << std::endl;

    Cache cache;
    cache.to_init.insert(cache.to_init.end(), cls_container->begin(), cls_container->end());
    this->init_cache(cache, cls_container);
    bool rebuilt = false;
    bool print_info = true;

    while(1)
    {
        /*
        std::cout << "outgoing" << std::endl;
        for(Cluster *cl : *cls_container)
        {
            std::vector<Cluster*> neighbours;
            cls_container->get_neighbours(neighbours, cl);
            std::cout << cl->to_string() << ", children: ";
            std::cout << neighbours.size();
            for(Cluster *neigh : neighbours)
            {
                std::cout << neigh->to_string() << ", ";
            }
            std::cout << std::endl;
        }
        */

        int num_cls = cls_container->size();

        Edge top;
        if(cache_enabled)
        {
            update_cache(cache, cls_container);
            top = get_next_pair_pq(cache);
        } else 
        {
            best_pair_iterate(top, cls_container);
        }

        //std::cout << "cl1 exists: " << cls_graph.find(std::get<1>(top)) << ", cl2 exists: " << cls_graph.find(std::get<2>(top)) << std::endl;
        if(print_info) std::cout << "number of clusters: " << cls_container->size() << ", score improvement: " << std::get<0>(top);

        //for(Cluster *cl : cls_graph) std::cout << cl->to_string() << std::endl;

        // if the score is worse then stop
        if(std::get<0>(top) >= 0)
        {
            if(rebuilt)
            {
                break;
            } else
            {
                cls_container->rebuild(cache.to_update);
                rebuilt = true;
            }
        } else
        {
            score += std::get<0>(top);
            //std::cout << "join: " << std::get<1>(top)->to_string() << " - " << std::get<2>(top)->to_string() << std::endl;
            join_clusters(top, cache, cls_container);
            rebuilt = false;
        }

        if(print_info) std::cout << ", pq size: " << cache.pq.size() << ", to_update.size(): " << cache.to_update.size() << std::endl;
    }

    std::cout << "cmp_count: " << cmp_count << std::endl;
    this->set_objective_value(score);

    std::unordered_map<Data*, std::string> cluster_map;
    int i = 0;
    for(auto &cl : *cls_container)
    {
        for(auto &elem : *cl)
        {
            cluster_map[elem] = this->generate_label(i);
        }
        i++;
    }

    //free_clusters();
    return cluster_map;
}

void Greedy_Joining::init_cache(Cache &cache, Cluster_Container *cls_container)
{
    if(parallel_enabled)
    {
        if(cache.to_init.size() == 1) init_cache_operation(cache, cls_container, nullptr, cache.to_init[0]);
        else init_cache_parallel(cache, cls_container);
    } else
    {
        init_cache_single(cache, cls_container);
    }
    
}

void Greedy_Joining::init_cache_single(Cache &cache, Cluster_Container *cls_container)
{
    for(Cluster *cl1 : cache.to_init)
    {
        init_cache_operation(cache, cls_container, nullptr, cl1);
    }
}

void Greedy_Joining::init_cache_parallel(Cache &cache, Cluster_Container *cls_container)
{
    int size = cache.to_init.size();
    std::mutex mtx;
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; i++)
    {
        int start = i*size/num_threads;
        int end = (i+1)*size/num_threads;
        if(start == end) continue;
        threads.emplace_back(&Greedy_Joining::init_cache_parallel_thread, this, std::ref(cache), std::ref(cls_container), &mtx, start, end);
    }

    for(std::thread &thread : threads)
    {
        thread.join();
    }
}

void Greedy_Joining::init_cache_parallel_thread(Cache &cache, Cluster_Container *cls_container, std::mutex *mtx, int start, int end)
{
    for(int i = start; i < end; i++)
    {
        Cluster *cl1 = cache.to_init[i];
        init_cache_operation(cache, cls_container, mtx, cl1);
    }
}

void Greedy_Joining::init_cache_operation(Cache &cache, Cluster_Container *cls_container, std::mutex *mtx, Cluster *cl1)
{
    float cl1_size = cl1->size();
    //std::cout << i++ << std::endl;
    std::vector<Cluster*> neighbours1;
    cls_container->get_neighbours(neighbours1, cl1);
    //std::cout << "num of neighbours1: " << neighbours1.size() << std::endl;
    //std::cout << cl1->to_string() << ", neighbours1: ";
    for(Cluster *cl2 : neighbours1)
    {
        //std::cout << cl2->to_string() << ", ";
        
        float cl2_size = cl2->size();
        float f_diff = Util_Cluster::f_diff(cl1, cl2);
        float d_diff = Util_Cluster::d_diff(cl1, cl2, distance);
        cmp_count++;

        if(f_diff+d_diff < 0)
        {
           if(mtx != nullptr)
           {
                std::lock_guard<std::mutex> lock(*mtx);
                cache.pq.emplace(f_diff+d_diff, cl1, cl2);
           } else
           {
                cache.pq.emplace(f_diff+d_diff, cl1, cl2);
           }
        }
    }
    //std::cout << std::endl;
}

void Greedy_Joining::update_cache(Cache &cache, Cluster_Container *cls_container)
{
    for(std::pair<Cluster*, Cluster*> &p : cache.to_update)
    {
        Cluster *cl1 = std::get<0>(p), *cl2 = std::get<1>(p);
        float score_diff = Util_Cluster::score_diff(cl1, cl2, distance);
        cmp_count++;
        
        if(score_diff < 0)
        {
            cache.pq.emplace(score_diff, cl1, cl2);
        }
    }

    cache.to_update.clear();
}

void Greedy_Joining::best_pair_iterate(Edge &best, Cluster_Container *cls_container)
{
    if(parallel_enabled) best_pair_iterate_parallel(best, cls_container);
    else best_pair_iterate_single(best, cls_container);
}

void Greedy_Joining::best_pair_iterate_single(Edge &best, Cluster_Container *cls_container)
{
    for(Cluster *cl1 : *cls_container)
    {
        best_pair_iterate_operation(best, cls_container, nullptr, cl1);
    }
}

void Greedy_Joining::best_pair_iterate_parallel(Edge &best, Cluster_Container *cls_container)
{
    int size = cls_container->size();
    std::mutex mtx;
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; i++)
    {
        int start = i*size/num_threads;
        int end = (i+1)*size/num_threads;
        if(start == end) continue;
        threads.emplace_back(&Greedy_Joining::best_pair_iterate_parallel_thread, this, std::ref(best), std::ref(cls_container), &mtx, start, end);
    }

    for(std::thread &thread : threads)
    {
        thread.join();
    }
}

void Greedy_Joining::best_pair_iterate_parallel_thread(Edge &best, Cluster_Container *cls_container, std::mutex *mtx, int start, int end)
{
    for(int i = start; i < end; i++)
    {
        Cluster *cl1 = (*cls_container)[i];
        best_pair_iterate_operation(best, cls_container, mtx, cl1);
    }
}

void Greedy_Joining::best_pair_iterate_operation(Edge &best, Cluster_Container *cls_container, std::mutex *mtx, Cluster *cl1)
{
    std::vector<Cluster*> neighbours1;
    cls_container->get_neighbours(neighbours1, cl1);

    for(Cluster *cl2 : neighbours1)
    {
        float f_diff = Util_Cluster::f_diff(cl1, cl2);
        float d_diff = Util_Cluster::d_diff(cl1, cl2, distance);
        cmp_count++;

        if(f_diff+d_diff < std::get<0>(best))
        {
            if(mtx != nullptr)
            {
                std::lock_guard<std::mutex> lock(*mtx);
                std::get<0>(best) = f_diff+d_diff;
                std::get<1>(best) = cl1;
                std::get<2>(best) = cl2;
            } else
            {
                std::get<0>(best) = f_diff+d_diff;
                std::get<1>(best) = cl1;
                std::get<2>(best) = cl2;
            }
            
        }
    }
}

Edge Greedy_Joining::get_next_pair_pq(Cache &cache)
{
    Edge top;

    do
    {
        if(cache.pq.empty()) return std::make_tuple(0, nullptr, nullptr);
        top = cache.pq.top();
        cache.pq.pop();
    } while(cache.invalid.count(std::get<1>(top)) == 1 || cache.invalid.count(std::get<2>(top)) == 1);

    return top;
}

Edge Greedy_Joining::get_next_pair_iterate(Cluster_Container *cls_container)
{
    Edge best;
    best_pair_iterate(best, cls_container);
    return best;
}

void Greedy_Joining::join_clusters(Edge &e, Cache &cache, Cluster_Container *cls_container)
{
    Cluster *cl1 = std::get<1>(e), *cl2 = std::get<2>(e);
    Cluster *cl_joined = cls_container->join(cl1, cl2, cache.to_update);

    if(cache_enabled)
    {
        cache.invalid.insert(cl1);
        cache.invalid.insert(cl2);
        cache.to_init.clear();
    }
}