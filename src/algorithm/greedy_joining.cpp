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

/**
 * @brief greedy joining implementation
 * It works by initializing each data element in its own cluster. A cost function indicates
 * the objective value of a clustering. Each join operation can either increase or decrease
 * the value. The algorithm chooses a pair of clusters at each iteration that improves the overall
 * cost the most (by decreasing the objective value), hence the 'greedy'. The algorithm continues
 * as long as the clustering can be improved. If any join operation would lead to a worse result
 * it stops. The selection of the next pair to join changes depending on the enabled options. It's
 * possible to use a cache, any of the available data structures, and parallelization.
 * @param input input data
 * @param dist distance threshold needed by the cost function
 * @return std::unordered_map<Data*, std::string> a map assigning each data element a string label
 */

std::unordered_map<Data*, std::string> Greedy_Joining::execute(std::vector<Data*> &input, float dist)
{
    double score = 0;
    Time timer;
    distance = dist;
    
    // clear cls_container
    for(Data *d : input) cls_container->add_data(d);
    timer.start();
    cls_container->init_clusters_fine_grained();
    std::cout << "time to build container structure: " << timer.stop() << std::endl;
    (*cls_container)[0]->to_string();

    // cache
    Cache cache;
    if(cache_enabled)
    {
        cache.to_init.insert(cache.to_init.end(), cls_container->begin(), cls_container->end());
        this->init_cache(cache, cls_container);
    }

    // union find
    Union_Find<Cluster*> uf(input.size()*2);
    for(auto &cl : *cls_container) uf.insert(cl);

    // 
    bool rebuilt = false;
    bool print_info = false;
    std::string out;

    while(1)
    {
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

        out += "number of clusters: " + std::to_string(cls_container->size()) + ", score improvement: " + std::to_string(std::get<0>(top));

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
            join_clusters(top, cache, cls_container, uf);
            rebuilt = false;
        }

        out += ", pq size: " + std::to_string(cache.pq.size()) + ", to_update.size(): " + std::to_string(cache.to_update.size());
        if(print_info) std::cout << out << std::endl;
        out.clear();
    }

    this->set_objective_value(score);

    // assign and generate labels to the data points using the union-find data structure
    std::unordered_map<Data*, std::string> data_to_label;
    std::unordered_map<Cluster*, int> root_to_label;
    
    int i = 0;
    for(Cluster *&cl : uf)
    {
        if(cl->data_size() == 0) continue;
        Cluster *&root = uf.find_(cl);
        if(root_to_label.count(root) == 0) root_to_label[root] = i++;
        for(Data *&d : *cl) data_to_label[d] = this->generate_label(root_to_label[root]);
    }

    // free all clusters on the heap
    for(Cluster *cl : cache.invalid) delete cl;
    cls_container->clear();
    return data_to_label;
}


/**
 * @brief initialize the empty cache with values
 * the number of inserted elements depends on the underlying container
 * create multiple threads if parallelization is enabled
 * 
 * @param cache cache struct
 * @param cls_container cluster container with all the available clusters
 */
void Greedy_Joining::init_cache(Cache &cache, Cluster_Container *cls_container)
{
    if(this->parallel_enabled())
    {
        if(cache.to_init.size() == 1) init_cache_operation(cache, cls_container, nullptr, cache.to_init[0]);
        else init_cache_parallel(cache, cls_container);
    } else
    {
        init_cache_single(cache, cls_container);
    }
    cache.to_init.clear();
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
    std::vector<Cluster*> neighbours1;
    cls_container->get_neighbours(neighbours1, cl1);
    for(Cluster *cl2 : neighbours1)
    {
        float cl2_size = cl2->size();
        float f_diff = Util_Cluster::f_diff(cl1, cl2);
        float d_diff = Util_Cluster::d_diff(cl1, cl2, distance);

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
}

/**
 * @brief after joining some nodes and edges are deleted from the container and some are added. This function
 * calculates and adds the missing values into the cache. For this, all the entries in cache.to_update are considered
 * 
 * @param cache 
 * @param cls_container 
 */
void Greedy_Joining::update_cache(Cache &cache, Cluster_Container *cls_container)
{
    for(std::pair<Cluster*, Cluster*> &p : cache.to_update)
    {
        Cluster *cl1 = std::get<0>(p), *cl2 = std::get<1>(p);
        float score_diff = Util_Cluster::score_diff(cl1, cl2, distance);
        
        if(score_diff < 0)
        {
            cache.pq.emplace(score_diff, cl1, cl2);
        }
    }

    cache.to_update.clear();
}


/**
 * @brief iterate through all possible edges of the container, calculate the objective value gain,
 * and select the best pair of clusters by storing them in the variable best. Parallelization is possible.
 * 
 * @param best the currently best edge/pair of clusters to join
 * @param cls_container 
 */
void Greedy_Joining::best_pair_iterate(Edge &best, Cluster_Container *cls_container)
{
    if(this->parallel_enabled()) best_pair_iterate_parallel(best, cls_container);
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


/**
 * @brief When making use of a cache, this function gets the next pair of clusters to join.
 * Some elements in the cache can be invalid if the nodes have been joined in previous iterations,
 * so the top element of the cache is removed until a valid edge has been found.
 * 
 * @param cache 
 * @return Edge 
 */
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

/**
 * @brief get the next pair to join by iteration. This should be used if a cache is not used.
 * 
 * @param cls_container 
 * @return Edge 
 */
Edge Greedy_Joining::get_next_pair_iterate(Cluster_Container *cls_container)
{
    Edge best;
    best_pair_iterate(best, cls_container);
    return best;
}


/**
 * @brief join the clusters in the edge e. And update all the other data structures, such as the cache
 * by invalidating the deleted clusters and marking the edges that need to be updated. After joining,
 * the union-find data structure is used to join the clusters.
 * 
 * @param e the edge that needs to be joined
 * @param cache 
 * @param cls_container 
 * @param uf union-find
 */
void Greedy_Joining::join_clusters(Edge &e, Cache &cache, Cluster_Container *cls_container, Union_Find<Cluster*> &uf)
{
    Cluster *cl1 = std::get<1>(e), *cl2 = std::get<2>(e);
    Cluster *cl_joined = cls_container->join(cl1, cl2, cache.to_update);
    uf.insert(cl_joined);
    uf.union_(cl_joined, cl1);
    uf.union_(cl_joined, cl2);

    if(cache_enabled)
    {
        cache.invalid.insert(cl1);
        cache.invalid.insert(cl2);
    } else
    {
        cache.to_update.clear();
    }
}