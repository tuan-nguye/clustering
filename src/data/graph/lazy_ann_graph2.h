#include <vector>
#include <list>
#include <cmath>
#include <random>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <set>

#include "data/graph/knn_graph.h"
#include "data/graph/sorted_graph.h"

extern int num_threads;

#ifndef __lazy_ann_graph2_include__
#define __lazy_ann_graph2_include__

template<typename T> class Lazy_ANN_Graph2: public KNN_Graph<T>
{
    private:
        // hierarchical layered graph
        std::vector<Sorted_Graph<T>*> hnsw;
        T enter_point;

        // global lock
        std::mutex global_lock;
        // hnsw structure lock
        std::mutex resize_lock;
        // layer lock
        std::unordered_map<int, std::mutex> layer_lock;
        // lock for each element/node
        std::unordered_map<T, std::mutex> elem_lock;

        // random number between [0, 1) generator
        std::random_device rd;
        std::mt19937 gen{rd()};
        std::uniform_real_distribution<float> uniform{0, 1};

        // parameters
        float m_l;              // optimal: 1/ln(m)
        int m;                  // argument: between 5 and 48
        int m_max;              // optimal 2*m
        int ef_construction;    // argument: can be calculated for each dataset

        // helper functions

        int calculate_level() { return int(std::roundf(std::fabs(-std::log(uniform(gen))*m_l))); }

        Sorted_Graph<T>* get_layer_graph_guarded(int l_c)
        {
            std::lock_guard<std::mutex> lock(resize_lock);
            return hnsw[l_c];
        }

        void add_layers(int num)
        {
            auto dist_func = this->get_distance_function();
            for(int i = 0; i < num; i++)
            {
                hnsw.push_back(new Sorted_Graph<T>(dist_func));
                layer_lock[i];
            }
        }

        void add_layers_guarded(int num)
        {
            std::lock_guard<std::mutex> lock(resize_lock);
            add_layers(num);
        }

        void get_children_guarded(int l_c, T &t, std::vector<T> &children)
        {
            Sorted_Graph<T> *layer_graph = get_layer_graph_guarded(l_c);
            std::lock_guard<std::mutex> lock(layer_lock[l_c]);
            layer_graph->get_children(children, t);
        }

        void add_edge_guarded(int l_c, T &t1, T &t2)
        {
            Sorted_Graph<T> *layer_graph = get_layer_graph_guarded(l_c);
            std::lock_guard<std::mutex> lock(layer_lock[l_c]);
            layer_graph->add_edge(t1, t2);
        }

        void add_node_guarded(int l_c, T &t)
        {
            Sorted_Graph<T> *layer_graph = get_layer_graph_guarded(l_c);
            layer_graph->add_node(t);
        }

        void lock_layer(int l_c)
        {
            std::lock_guard<std::mutex> lock_map(resize_lock);
            std::string out = "lock layer " + std::to_string(l_c) + "\n";
            //std::cout << out;
            std::cout.flush();
            layer_lock[l_c].lock();
        }

        void unlock_layer(int l_c)
        {
            std::lock_guard<std::mutex> lock_map(resize_lock);
            std::string out = "unlock layer " + std::to_string(l_c) + "\n";
            //std::cout << out;
            std::cout.flush();
            layer_lock[l_c].unlock();
        }

        void lock_element(T &t)
        {
            std::string out = "lock element " + t->to_string() + "\n";
            //std::cout << out;
            std::cout.flush();
            elem_lock[t].lock();
        }

        void unlock_element(T &t)
        {
            elem_lock[t].unlock();
            std::string out = "unlock element " + t->to_string() + "\n";
            //std::cout << out;
            std::cout.flush();
        }

        // functions for inserting values and building hnsw

        // insert element into hierachical structured layer graphs
        void insert(T &t, std::mutex *mtx)
        {
            lock_element(t);
            int l = calculate_level();

            std::unique_lock<std::mutex> temp_global(global_lock);
            int L = top_layer();
            if(l <= L)
            {
                temp_global.unlock();
            }

            std::vector<T> nearest_elements;
            //std::cout << "insert enter, (l, L) = " << l << ", " << L << std::endl;

            T ep = enter_point;

            for(int l_c = L; l_c >= l+1; l_c--)
            {
                search_layer(t, ep, 1, l_c, nearest_elements);
                T &back = nearest_elements.back();
                
                if(back != t && this->get_distance_function()(t, back) < this->get_distance_function()(t, ep)) ep = back;
                nearest_elements.clear();
            }

            for(int l_c = std::min(l, L); l_c >= 0; l_c--)
            {
                search_layer(t, ep, ef_construction, l_c, nearest_elements);
                std::vector<T> neighbours;
                select_neighbours(t, nearest_elements, m, neighbours);

                //lock_layer(l_c);
                Sorted_Graph<T> *layer_graph = get_layer_graph_guarded(l_c);
                layer_graph->add_node(t);
                for(T &tn : neighbours)
                {
                    //lock_element(tn);
                    /*if(layer_graph.number_of_children(tn) < m_max)
                    {
                        layer_graph.add_edge(t, tn);
                    } else
                    {
                        T &tnc = layer_graph.last_child(tn);
                        if(tnc == t) continue;
                        auto distance = this->get_distance_function();
                        if(distance(tn, t) < distance(tn, tnc))
                        {
                            layer_graph.add_edge(tn, t);
                            std::lock_guard<std::mutex> lock_tnc(elem_lock[tnc]);
                            layer_graph.remove_edge(tn, tnc);
                        }
                    }*/
                    layer_graph->add_edge(t, tn);
                    if(layer_graph->number_of_children(tn) > m_max)
                    {
                        T &tnc = layer_graph->last_child(tn);
                        //if(tnc != t) lock_element(tnc);
                        layer_graph->remove_edge(tn, tnc);
                        //unlock_element(tnc);
                    }
                    //unlock_element(tn);
                }
                //unlock_layer(l_c);
                
                std::vector<T> temp;
                std::unordered_set<T> exclude;
                this->top_k(1, t, nearest_elements, exclude, temp);
                if(this->get_distance_function()(t, temp[0]) < this->get_distance_function()(t, ep)) ep = temp[0];
                // element_lock[nearest_elements[0]]
                nearest_elements.clear();
            }

            if(l > L)
            {
                add_layers(l-L);
                for(int l_c = l; l_c > L; l_c--) hnsw[l_c]->add_node(t);
                enter_point = t;
            }

            unlock_element(t);
        }

        // greedy dfs that searches for nearest neighbours ands adds them
        // to result_vec, max number of neighbours limited by ef
        void search_layer(T &t, T &ep, int ef, int l_c, std::vector<T> &result_vec)
        {
            //lock_layer(l_c);
            std::unordered_set<T> visited = {ep};
            std::pair<float, T> start(this->get_distance_function()(t, ep), ep);
            std::priority_queue<std::pair<float, T>, std::vector<std::pair<float, T>>, std::greater<std::pair<float, T>>> candidates;
            candidates.push(start);
            std::priority_queue<std::pair<float, T>> nearest_neighbours;
            nearest_neighbours.push(start);
            Sorted_Graph<T> *layer_graph = get_layer_graph_guarded(l_c);

            while(!candidates.empty())
            {
                std::pair<float, T> top_pair = candidates.top();
                candidates.pop();
                std::pair<float, T> last_pair = nearest_neighbours.top();
                if(std::get<0>(top_pair) > std::get<0>(last_pair)) break;

                T top = std::get<1>(top_pair);
                std::vector<T> children;
                //lock_element(top);
                layer_graph->get_children(children, top);
                //unlock_element(top);
                for(T &tn : children)
                {
                    if(visited.count(tn) != 0) continue;
                    visited.insert(tn);
                    float distance = this->get_distance_function()(t, tn);
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
            //unlock_layer(l_c);
        }

        void select_neighbours(T &t, std::vector<T> &candidates, int m, std::vector<T> &top_m)
        {
            std::unordered_set<T> empty;
            this->top_k(m, t, candidates, empty, top_m);
        }

        // search multilayer graph hnsw for its k nearest neighbours
        void knn_search(T &t, std::vector<T> &neighbours)
        {
            auto cmp = this->cmp_function_bool(t);
            std::set<T, decltype(cmp)> nearest_neighbours(cmp);
            std::vector<T> temp_vec;
            T ep = enter_point;
            int L = top_layer();

            for(int l_c = L; l_c >= 1; l_c--)
            {
                search_layer(t, ep, 1, l_c, temp_vec);
                nearest_neighbours.insert(temp_vec[0]);
                ep = *nearest_neighbours.begin();
                temp_vec.clear();
            }

            search_layer(t, ep, ef_construction, 0, temp_vec);
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
            for(int i = 0; (it != nearest_neighbours.end()) && (i < k); i++) neighbours.push_back(*it++);
        }

        int top_layer() { return hnsw.size()-1; }

        void print_debug()
        {
            std::string out = "layers{";
            for(int i = 0; i < hnsw.size(); i++)
            {
                if(i != 0) out += ", ";
                out += "{" + std::to_string(i) + ": (" + std::to_string(hnsw[i]->size()) + ", " + std::to_string(hnsw[i]->size_edges()) + ")}";
            }
            out += "}";
            //std::cout << out;
            std::cout.flush();
        }
    protected:
        // add node to hierarchy
        void add_edges_operation(T &t, std::mutex *mtx)
        {
            //if(mtx != nullptr) mtx->lock();
            insert(t, mtx);
            //if(mtx != nullptr) mtx->unlock();
            return;
            if(mtx != nullptr) mtx->lock();
            
            for(int i = 0; i < hnsw.size(); i++)
            {
                //std::cout << "layer " << i << std::endl;
                get_layer_graph_guarded(i)->print_structure();
            }
            if(mtx != nullptr) mtx->unlock();
        }

        void build_graph()
        {
            /*std::unordered_map<int, int> count;
            for(int i = 0; i < 1000; i++) count[calculate_level()]++;

            for(auto &e : count) std::cout << e.first << ": " << e.second << "\n";
            std::cout.flush();

            return;*/
            for(T &t : this->get_all_elements()) elem_lock[t];
            Auto_Edge_Graph<T>::build_graph();
            // call knn_search for every node
            // parallel or single threaded
            knn_search_and_add();
            print_debug();
        }

        void knn_search_and_add()
        {
            if(this->get_parallel()) knn_search_parallel();
            else knn_search_single();
        }

        void knn_search_single()
        {
            for(T &t : this->get_all_elements())
            {
                knn_search_operation(t, nullptr);
            }
        }

        void knn_search_parallel()
        {
            int size = this->size();
            std::mutex mtx;
            std::vector<std::thread> threads;

            for(int i = 0; i < num_threads; i++)
            {
                int start = i*size/num_threads;
                int end = (i+1)*size/num_threads;
                if(start == end) continue;
                threads.emplace_back(&Lazy_ANN_Graph2::knn_search_parallel_thread, this, &mtx, start, end);
            }

            for(std::thread &thread : threads)
            {
                thread.join();
            }
        }

        void knn_search_parallel_thread(std::mutex *mtx, int start, int end)
        {
            Maptor<T> &elements = this->get_all_elements();
            std::vector<T> nn;

            for(int i = start; i < end; i++)
            {
                knn_search_operation(elements[i], mtx);
            }
        }

        void knn_search_operation(T &t, std::mutex *mtx)
        {
            std::vector<T> knn;
            knn_search(t, knn);
            if(mtx == nullptr)
            {
                for(T &tk : knn) this->add_edge_directed(t, tk);
            } else
            {
                std::lock_guard<std::mutex> lock(*mtx);
                for(T &tk : knn) this->add_edge_directed(t, tk);
            }
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
        Lazy_ANN_Graph2(int k, int m, int ef_construction, std::function<float(T&, T&)> cmp):
        KNN_Graph<T>(k, cmp),
        m(m), m_l(1/std::log(m)), m_max(2*m), ef_construction(ef_construction)
        {
            //hnsw.reserve(1000);
        }

        void rebuild(std::vector<std::pair<T, T>> &to_update)
        {
            // clear current state
            this->clear_edges();
            clear_hnsw();

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

        void clear_hnsw()
        {
            for(Sorted_Graph<T> *sg : hnsw) delete sg;
            hnsw.clear();
            elem_lock.clear();
        }
};

#endif