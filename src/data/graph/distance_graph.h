#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>

#include "data/graph/auto_edge_graph.h"


#ifndef __distance_graph_include__
#define __distance_graph_include__

template<typename T> class Distance_Graph: public Auto_Edge_Graph<T>
{
    private:
        float (*cmp)(T&, T&);
        float dist;
        
    protected:
        void add_edge_on_condition(T &t1, T &t2, std::mutex &mtx)
        {
            if(cmp(t1, t2) > dist) return;
            std::lock_guard<std::mutex> lock(mtx);
            Graph<T>::add_edge(t1, t2);
        }
    public:
        Distance_Graph(float distance, float(*comparator)(T&, T&)): dist(distance), cmp(comparator) {}

        /*
        create new node from two other nodes
        children of new node is union of the children
        of the two others

        also remove reference to node by removing edge
        before removing the node, otherwise segfault
        */
        void combine_nodes_into(T &c, T &t1, T &t2)
        {
            Graph<T>::add_node(c);
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            std::unordered_set<Node<T>*> updated_children;
            this->remove_edge(t1, t2);

            for(Node<T> *next1 : n1->get_children())
            {
                updated_children.insert(next1);
            }

            for(Node<T> *next2 : n2->get_children())
            {
                updated_children.insert(next2);
            }

            for(Node<T> *next : updated_children)
            {
                this->add_edge(c, next->get_value());
            }

            this->remove_node(t1);
            this->remove_node(t2);
        }
};

#endif