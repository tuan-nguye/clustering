#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "data/graph/sorted_node.h"
#include "data/structures/maptor.h"

#ifndef __knn_graph_include__
#define __knn_graph_include__

/**
 * @brief K-Nearest-Neighbours Graph. Edges are automatically added.
 * 
 * @tparam T 
 */
template<typename T> class KNN_Graph: public Auto_Edge_Graph<T>
{
    private:
        // comparison function for evaluating distance between two clusters
        std::function<float(T&, T&)> distance;
        // distance constant
        float dist;
        // k constant, number of neighbours
        int k;
        // map storing all incoming neighbours of a value
        std::unordered_map<T, std::unordered_set<T>> incoming;

    protected:
        /**
         * @brief A function that tries to add outgoing edges after joining t1 and t2 into c.
         * The idea is to save time from searching through all nodes by reusing the outgoing edges
         * of t1 and t2.
         * 
         * @param c 
         * @param t1 
         * @param t2 
         * @param to_update 
         */
        virtual void update_outgoing_edges(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::vector<T> children1, children2;
            this->get_children(children1, t1);
            this->get_children(children2, t2);
            Maptor<T> outgoing;
            outgoing.reserve(2*k);
            float b_limit = 0.0f;
            if(children1.size() != 0) b_limit += distance(t1, children1.back());
            if(children2.size() != 0) b_limit += distance(t2, children2.back());

            // outgoing

            for(T &tn1 : children1)
            {
                incoming[tn1].erase(t1);
                if(distance(c, tn1) > b_limit) continue;
                outgoing.push_back(tn1);
            }

            for(T &tn2 : children2)
            {
                incoming[tn2].erase(t2);
                if(distance(c, tn2) > b_limit) continue;
                outgoing.push_back(tn2);
            }
            outgoing.erase(t1);
            outgoing.erase(t2);
            
            if(outgoing.size() >= k)
            {
                std::vector<T> topk;
                std::unordered_set<T> empty;
                top_k(k, c, outgoing.get_vector(), empty, topk);
                for(T &t : topk) add_edge_directed_update(c, t, to_update);
            } else
            {
                update_outgoing_edges_all(c, t1, t2, to_update);
            }
        }

        /**
         * @brief if not enough outgoing edges could be added, search through all nodes
         * 
         * @param c 
         * @param t1 
         * @param t2 
         * @param to_update 
         */
        virtual void update_outgoing_edges_all(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::unordered_set<T> exclude = {c, t1, t2};
            std::vector<T> top_k_elements;
            this->top_k(k, c, this->get_all_elements().get_vector(), exclude, top_k_elements);
            for(T &t : top_k_elements)
                add_edge_directed_update(c, t, to_update);
        }

        /**
         * @brief when joining t1 and t2, the according nodes are removed from the graph. Incoming
         * edges are going to be removed and therefore have to be replaced. If the distance
         * towards the newly joined node c is smaller than towards either t1 or t2, a new edge to
         * c can be used to replace the edge.
         * 
         * @param c 
         * @param t1 
         * @param t2 
         * @param to_update 
         */
        void update_incoming_edges(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::unordered_set<T> excluded_values = {c, t1, t2};
            Maptor<T> to_replace;
            to_replace.reserve(incoming[t1].size()+incoming[t2].size());

            for(T tn1 : incoming[t1])
            {
                to_replace.push_back(tn1);
                Graph<T>::remove_edge_directed(tn1, t1);
            }

            for(T tn2 : incoming[t2])
            {
                to_replace.push_back(tn2);
                Graph<T>::remove_edge_directed(tn2, t2);
            }

            to_replace.erase(t1);
            to_replace.erase(t2);
            for(T &tn : to_replace)
            {
                if(this->number_of_children(tn) == 0) continue;

                T &last = this->last_child(tn);
                if(distance(tn, c) < distance(tn, last)) add_edge_directed_update(tn, c, to_update);
                else replace_incoming_edges_all(tn, excluded_values, to_update);
            }
        }

        /**
         * @brief searches through all nodes to find an edge to replace the deleted incoming edge.
         * 
         * @param t 
         * @param exclude 
         * @param to_update 
         */
        virtual void replace_incoming_edges_all(T &t, std::unordered_set<T> exclude, std::vector<std::pair<T, T>> &to_update)
        {
            // erase elements that are children already or itself
            std::vector<T> children;
            this->get_children(children, t);
            for(T &t : children) exclude.insert(t);
            exclude.insert(t);
            
            if(this->size() <= exclude.size()) return;
            std::vector<T> top1;
            top_k(1, t, this->get_all_elements().get_vector(), exclude, top1);
            add_edge_directed_update(t, top1[0], to_update);
        }

        /**
         * @brief Create a node object, factory method. Sorted by distance in ascending order
         * 
         * @param t 
         * @return Node<T>* 
         */
        Node<T>* create_node(T &t)
        {
            return new Sorted_Node<T>(t, distance);
        }

        /**
         * @brief find the closest k elements to t and add them as children
         * 
         * @param t 
         * @param mtx 
         */
        virtual void add_edges_operation(T &t, std::mutex *mtx)
        {
            std::vector<T> topk;
            std::unordered_set<T> exclude = {t};
            top_k(k, t, this->get_all_elements().get_vector(), exclude, topk);

            if(mtx == nullptr) 
            {
                for(T &tn : topk) this->add_edge_directed(t, tn);
            } else
            {
                std::lock_guard<std::mutex> lock(*mtx);
                for(T &tn : topk) this->add_edge_directed(t, tn);
            }
        }

        // returns cmp function for sorted data structures
        // compares distance of two values to &t
        std::function<float(T&, T&)> cmp_function(T &t)
        {
            //std::function<float(T&, T&)> cmp_captured = distance;
            std::function<float(T&, T&)> eval_nearest_neighbour = [&t, this](T &t1, T &t2) -> float
            {
                return this->distance(t, t1) - this->distance(t, t2);
            };
            return eval_nearest_neighbour;
        }

        // returns cmp function for sorted data structures
        // returns bool whether first element is smaller than second
        std::function<bool(T, T)> cmp_function_bool(T &t)
        {
            std::function<bool(T, T)> eval_compare = [&t, this](T t1, T t2) -> float
            {
                return this->distance(t, t1) < this->distance(t, t2);
            };

            return eval_compare;
        }

        // finds the top k elements according to the comparison function
        // and stores them in top_elements
        void top_k(int k, T &t, std::vector<T> &candidates, std::unordered_set<T> &exclude, std::vector<T> &top_elements)
        {
            Sorted_Vector<T> top_kek(cmp_function(t));

            for(T &tn : candidates)
            {
                if(exclude.count(tn) != 0) continue;
                if(top_kek.size() < k)
                {
                    top_kek.push(tn);
                } else
                {
                    T &last = top_kek.back();
                    if(distance(t, tn) < distance(t, last))
                    {
                        top_kek.pop_back();
                        top_kek.push(tn);
                    }
                }
            }

            top_elements.reserve(k);
            for(T &tn : top_kek)
            {
                top_elements.push_back(tn);
            }
            //std::cout << "k: " << k << ", actual: " << top_elements.size() << std::endl;
        }

        /**
         * @brief Get the k constant value
         * 
         * @return int 
         */
        int get_k() { return k; }
        
        /**
         * @brief Get the distance threshold called d
         * 
         * @return int 
         */
        int get_d() { return dist; }

        /**
         * @brief Get the distance function
         * 
         * @return std::function<float(T&, T&)>& 
         */
        std::function<float(T&, T&)>& get_distance_function() { return distance; }
    public:
        /**
         * @brief Construct a new knn graph object
         * 
         * @param k number of children each node has
         * @param distance threshold used for the distance function
         */
        KNN_Graph(int k, std::function<float(T&, T&)> distance): k(k), distance(distance) {}

        // 1. create new joined node
        // 2. iterate through all outgoing neighbours of t1, t2
        //    and add if c is <= b_limit
        // 3. if size is less than k, iterate through all nodes
        //    to find the rest
        // 4. iterate through all incoming neighbours of t1, t2,
        //    both will be deleted so replace them
        //    if possible add c, if not then iterate through all
        //    to find the one to replace it with
        virtual void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            // add new joined node c and add its new edges
            this->add_node(c);
            update_outgoing_edges(c, t1, t2, to_update);
            update_incoming_edges(c, t1, t2, to_update);
            
            // remove the original nodes from the graph
            this->remove_edge(t1, t2);
            this->remove_node(t1);
            this->remove_node(t2);
        }

        // override base function to update incoming map
        void remove_node(T &t)
        {
            incoming.erase(t);
            Graph<T>::remove_node(t);
        }

        // override base function to update incoming map
        void add_edge_directed(T &t1, T &t2)
        {
            incoming[t2].insert(t1);
            Graph<T>::add_edge_directed(t1, t2);
        }

        // add a new directed edge to the graph and save in to_update
        void add_edge_directed_update(T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            add_edge_directed(t1, t2);
            to_update.emplace_back(t1, t2);
        }

        // override base function to update incoming map
        void remove_edge_directed(T &t1, T &t2)
        {
            if(incoming.find(t2) != incoming.end()) incoming[t2].erase(t1);
            Graph<T>::remove_edge_directed(t1, t2);
        }

        // get neighbours of an object t and store them in vec
        void get_neighbours(std::vector<T> &vec, T &t)
        {
            this->get_children(vec, t);
        }

        // clear the entire graph
        void clear()
        {
            incoming.clear();
            Auto_Edge_Graph<T>::clear();
        }

        // clear all edges of the graph
        void clear_edges()
        {
            incoming.clear();
            Auto_Edge_Graph<T>::clear_edges();
        }
};

#endif