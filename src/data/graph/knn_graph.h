#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>

#include "data/graph/sorted_node.h"
#include "data/structures/maptor.h"

#ifndef __knn_graph__
#define __knn_graph__


template<typename T> class KNN_Graph: public Auto_Edge_Graph<T>
{
    private:
        float (*cmp)(T&, T&, float);
        float dist;
        int k;
        std::unordered_map<Node<T>*, std::vector<Node<T>*>> incoming;

        // add edge from t1 -> t2
        void add_edge_limit_k(T &t1, T &t2)
        {
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            std::vector<Node<T>*> children1 = n1->get_children();
            
            if(children1.size() == k)
            {
                Node<T> *last = children1.back();
                this->remove_edge_directed(t1, last->get_value());
                std::vector<Node<T>*> &incoming1 = incoming[n1];
                incoming1.erase(std::remove(incoming1.begin(), incoming1.end(), last), incoming1.end());
            }

            this->add_edge_directed(t1, t2);
            incoming[n2].push_back(n1);
        }

        void update_outgoing_edges()
        {

        }

        void update_incoming_edges()
        {

        }
    protected:
        Node<T>* create_node(T &t)
        {
            return new Sorted_Node<T>(t, cmp, dist);
        }

        void add_edge_on_condition(T &t1, T &t2, std::mutex &mtx)
        {
            // calculate score_diff, add when better than last element
            // if added, then cut size of children to k
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            std::vector<Node<T>*> children1 = n1->get_children(), children2 = n2->get_children();
            float score_diff = cmp(t1, t2, dist);

            if(children1.size() < k || score_diff < cmp(t1, children1.back()->get_value(), dist))
            {
                std::lock_guard<std::mutex> lock(mtx);
                add_edge_limit_k(t1, t2);
            }

            if(children2.size() < k || score_diff < cmp(t2, children2.back()->get_value(), dist))
            {
                std::lock_guard<std::mutex> lock(mtx);
                add_edge_limit_k(t2, t1);
            }
        }
    public:
        KNN_Graph(int k, float distance, float(*comparator)(T&, T&, float)): k(k), dist(distance), cmp(comparator) {}

        // 1. create new joined node
        // 2. iterate through all outgoing neighbours of t1, t2
        //    and add if c is <= b_limit
        // 3. if size is less than k, iterate through all nodes
        //    to find the rest
        // 4. iterate through all incoming neighbours of t1, t2,
        //    both will be deleted so replace them
        //    if possible add c, if not then iterate through all
        //    to find the one to replace it with

        void combine_nodes_into(T &c, T &t1, T &t2)
        {
            Graph<T>::add_node(c);
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            Maptor<T> outgoing;
            this->remove_edge(t1, t2);
            float b_limit = cmp(t1, n1->get_children().back()->get_value(), dist) + cmp(t2, n2->get_children().back()->get_value(), dist);

            // outgoing

            for(Node<T> *next1 : n1->get_children())
            {
                T &t = next1->get_value();
                if(cmp(c, t, dist) > b_limit) continue;
                outgoing.push_back(t);
            }

            for(Node<T> *next2 : n2->get_children())
            {
                T &t = next2->get_value();
                if(cmp(c, t, dist) > b_limit) continue;
                outgoing.push_back(t);
            }

            if(outgoing.size() >= k)
            {
                for(T &t : outgoing) this->add_edge_limit_k(c, t);
            } else
            {
                for(T &t : this->get_all_elements()) this->add_edge_limit_k(c, t); 
            }

            // incoming

            for(Node<T> *n : incoming[n1])
            {
                if(cmp(n->get_value(), c, dist) <= cmp(n->get_value(), n->get_children().back()->get_value(), dist)) this->add_edge_directed(n->get_value(), c);
                else for(T &tnn : this->get_all_elements()) add_edge_limit_k(n->get_value(), tnn);
                this->remove_edge_directed(n->get_value(), t1);
            }
            incoming.erase(n1);
            this->remove_node(t1);

            for(Node<T> *n : incoming[n2])
            {
                if(cmp(n->get_value(), c, dist) <= cmp(n->get_value(), n->get_children().back()->get_value(), dist)) this->add_edge_directed(n->get_value(), c);
                else for(T &tnn : this->get_all_elements()) add_edge_limit_k(n->get_value(), tnn);
                this->remove_edge_directed(n->get_value(), t2);
            }
            incoming.erase(n2);
            this->remove_node(t2);
        }
};

#endif