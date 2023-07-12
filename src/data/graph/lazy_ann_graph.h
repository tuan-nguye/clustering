#include <vector>
#include <cmath>
#include <random>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <set>

#include "data/graph/knn_graph.h"

extern int num_threads;

#ifndef __lazy_ann_graph_include__
#define __lazy_ann_graph_include__

template<typename T> class Lazy_ANN_Graph: public KNN_Graph<T>
{
    private:
        // hierarchical layered graph
        std::vector<Graph<T>> hnsw;
        T enter_point;

        // lock for each layer
        std::vector<std::mutex*> mtxs;
        // currently values being inserted
        std::unordered_set<T> modifying;

        // random number between [0, 1) generator
        std::random_device rd;
        std::mt19937 gen{rd()};
        std::uniform_real_distribution<float> uniform{0, 1};

        // parameters
        float m_l;              // optimal: 1/ln(m)
        int m;                  // argument: between 5 and 48
        int m_max;              // optimal 2*m
        int ef_construction;    // argument: can be calculated for each dataset

        int calculate_level() { return int(std::roundf(std::fabs(-std::log(uniform(gen))*m_l))); }

        // insert element into hierachical structured layer graphs
        void insert(T &t, std::mutex *mtx)
        {
            std::vector<T> nearest_elements;
            int L = top_layer();
            int l = calculate_level();
            //std::cout << "insert enter, (l, L) = " << l << ", " << L << std::endl;

            // if hnsw is empty insert t and return
            if(mtx != nullptr) mtx->lock();
            if(hnsw.size() == 0)
            {
                hnsw.emplace_back();
                mtxs.push_back(new std::mutex());
                hnsw[0].add_node(t);
                enter_point = t;
                if(mtx != nullptr) mtx->unlock();
                return;
            }
            if(mtx != nullptr) mtx->unlock();
            T ep = enter_point;

            for(int l_c = L; l_c >= l+1; l_c--)
            {
                T back;
                bool found = false;

                if(mtx == nullptr)
                {
                    search_layer(t, ep, 1, hnsw[l_c], nearest_elements);
                    back = nearest_elements.back();
                    found = true;
                } else
                {
                    mtx->lock();
                    Graph<T> &layer_graph = hnsw[l_c];
                    mtx->unlock();
                    mtxs[l_c]->lock();
                    search_layer(t, ep, num_threads+1, layer_graph, nearest_elements);
                    mtxs[l_c]->unlock();

                    std::lock_guard<std::mutex> lock_ep(*mtx);
                    for(T &tep : nearest_elements)
                    {
                        if(modifying.count(tep) == 0)
                        {
                            back = tep;
                            found = true;
                            break;
                        }
                    }
                }
                
                if(found && this->get_cmp()(t, back) < this->get_cmp()(t, ep)) ep = back;
                nearest_elements.clear();
            }

            for(int l_c = std::min(l, L); l_c >= 0; l_c--)
            {
                if(mtx != nullptr) mtx->lock();
                Graph<T> &layer_graph = hnsw[l_c];
                if(mtx != nullptr) mtx->unlock();
                mtxs[l_c]->lock();
                search_layer(t, ep, ef_construction, layer_graph, nearest_elements);
                std::vector<T> neighbours;
                select_neighbours(t, nearest_elements, m, neighbours);

                // lock here
                //if(mtx != nullptr) mtx->lock();
                layer_graph.add_node(t);
                for(T &tn : neighbours)
                {
                    // get last child and pop if > m_max to improve performance
                    // no need for search_neighbours() function
                    std::vector<T> tn_conn;
                    // lock; bad sync, use nodes with sorted children instead
                    layer_graph.add_edge(t, tn);
                    layer_graph.get_children(tn_conn, tn);

                    if(tn_conn.size() > m_max)
                    {
                        std::vector<T> top_m_max;
                        select_neighbours(tn, tn_conn, m_max, top_m_max);
                        std::unordered_set<T> top_set(top_m_max.begin(), top_m_max.end());

                        for(T &tnc : tn_conn)
                        {
                            if(top_set.count(tnc) == 0) layer_graph.remove_edge(tn, tnc);
                        }
                    }
                }
                mtxs[l_c]->unlock();
                //if(mtx != nullptr) mtx->unlock();
                std::vector<T> temp;
                this->top_k(num_threads+1, t, nearest_elements, temp);
                if(mtx != nullptr) std::lock_guard<std::mutex> lock_ep(*mtx);
                for(T &tep : temp)
                {
                    if(modifying.count(tep) == 0)
                    {
                        ep = tep;
                        break;
                    }
                }
                
                nearest_elements.clear();
            }

            if(mtx != nullptr) std::lock_guard<std::mutex> lock(*mtx);
            if(l > top_layer())
            {
                hnsw.resize(l+1);
                for(int l_c = l; l_c > L; l_c--)
                {
                    hnsw[l_c].add_node(t);
                    mtxs.push_back(new std::mutex());
                }
                enter_point = t;
            }
        }

        // greedy dfs that searches for nearest neighbours ands adds them
        // to result_vec, max number of neighbours limited by ef
        void search_layer(T &t, T &ep, int ef, Graph<T> &layer_graph, std::vector<T> &result_vec)
        {
            std::unordered_set<T> visited = {ep};
            std::pair<float, T> start(this->get_cmp()(t, ep), ep);
            std::priority_queue<std::pair<float, T>, std::vector<std::pair<float, T>>, std::greater<std::pair<float, T>>> candidates;
            candidates.push(start);
            std::priority_queue<std::pair<float, T>> nearest_neighbours;
            nearest_neighbours.push(start);

            while(!candidates.empty())
            {
                std::pair<float, T> top_pair = candidates.top();
                candidates.pop();
                std::pair<float, T> last_pair = nearest_neighbours.top();
                if(std::get<0>(top_pair) > std::get<0>(last_pair)) break;

                T top = std::get<1>(top_pair);
                std::vector<T> children;
                layer_graph.get_children(children, top);
                for(T &tn : children)
                {
                    if(visited.count(tn) != 0) continue;
                    visited.insert(tn);
                    float distance = this->get_cmp()(t, tn);
                    last_pair = nearest_neighbours.top();

                    if(nearest_neighbours.size() < ef || distance <= std::get<0>(last_pair))
                    {
                        candidates.emplace(distance, tn);
                        nearest_neighbours.emplace(distance, tn);
                        if(nearest_neighbours.size() > ef) nearest_neighbours.pop();
                    }
                }
            }

            while(!nearest_neighbours.empty()) 
            {
                result_vec.push_back(std::get<1>(nearest_neighbours.top()));
                nearest_neighbours.pop();
            }
        }

        void select_neighbours(T &t, std::vector<T> &candidates, int m, std::vector<T> &top_m)
        {
            this->top_k(m, t, candidates, top_m);
        }

        // search multilayer graph hnsw for its k nearest neighbours
        void knn_search_operation(T &t, std::vector<T> &neighbours)
        {
            auto cmp = this->cmp_function_bool(t);
            std::set<T, decltype(cmp)> nearest_neighbours(cmp);
            std::vector<T> temp_vec;
            T ep = enter_point;
            int L = top_layer();

            for(int l_c = L; l_c >= 1; l_c--)
            {
                search_layer(t, ep, 1, hnsw[l_c], temp_vec);
                nearest_neighbours.insert(temp_vec[0]);
                ep = *nearest_neighbours.begin();
                temp_vec.clear();
            }

            search_layer(t, ep, ef_construction, hnsw[0], temp_vec);
            for(T &tt : temp_vec)
            {
                nearest_neighbours.insert(tt);
            }

            // remove itself
            nearest_neighbours.erase(t);

            // write to output vector
            auto it = nearest_neighbours.begin();
            int k = this->get_k();
            neighbours.reserve(k);
            for(int i = 0; i < k; i++) neighbours.push_back(*it++);
        }

        int top_layer() { return hnsw.size()-1; }

        void print_debug()
        {
            std::cout << "layers{";
            for(int i = 0; i < hnsw.size(); i++)
            {
                if(i != 0) std::cout << ", ";
                std::cout << "{" << i << ": (" << hnsw[i].size() << ", " << hnsw[i].size_edges() << ")}";
            }
            std::cout << "}" << std::endl;
        }
    protected:
        // add node to hierarchy
        void add_edges_operation(T &t, std::mutex *mtx)
        {
            //if(mtx != nullptr) mtx->lock();
            if(mtx != nullptr)
            {
                std::lock_guard<std::mutex> lock(*mtx);
                modifying.insert(t);
            }
            insert(t, mtx);
            if(mtx != nullptr)
            {
                std::lock_guard<std::mutex> lock(*mtx);
                modifying.erase(t);
            }
            //if(mtx != nullptr) mtx->unlock();
            print_debug();
        }

        void build_graph()
        {
            /*std::unordered_map<int, int> count;
            for(int i = 0; i < 1000; i++) count[calculate_level()]++;

            for(auto &e : count) std::cout << e.first << ": " << e.second << "\n";
            std::cout.flush();

            return;*/
            Auto_Edge_Graph<T>::build_graph();
            // call knn_search for every node
            // parallel or single threaded
            knn_search();
        }

        void knn_search()
        {
            if(this->get_parallel()) knn_search_parallel();
            else knn_search_single();
        }

        void knn_search_single()
        {
            for(T &t : this->get_all_elements())
            {
                std::vector<T> knn;
                knn_search_operation(t, knn);
                for(T &tk : knn) this->add_edge_limit_k(t, tk);
            }
        }

        void knn_search_parallel()
        {

        }

        void update_outgoing_edges_all(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            // leave empty, lazy implementation
        }

        void replace_incoming_edges_all(T &t, T &tn, std::vector<T> &children, Maptor<T> &candidates, std::vector<std::pair<T, T>> &to_update)
        {
            // leave empty, lazy implementation
        }
    public:
        Lazy_ANN_Graph(int k, int m, int ef_construction, std::function<float(T&, T&)> cmp): KNN_Graph<T>(k, cmp), m(m), m_l(1/std::log(m)), m_max(2*m), ef_construction(ef_construction)
        {
            hnsw.reserve(1000);
            mtxs.reserve(1000);
        }

        void rebuild(std::vector<std::pair<T, T>> &to_update)
        {
            // clear current state
            this->clear_edges();
            hnsw.clear();
            mtxs.clear();

            // rebuild
            this->build_graph();

            // add pairs to update vector
            for(T &t : this->get_all_elements())
            {
                std::vector<T> children;
                this->get_neighbours(children, t);
                for(T &tc : children)
                {
                    to_update.emplace_back(t, tc);
                }
            }
        }
};

#endif