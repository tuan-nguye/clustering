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
        std::unordered_set<T> visited;
        
    protected:
        void add_edges_operation(T &t, std::mutex *mtx)
        {
            for(T &tn : this->get_all_elements())
            {
                bool done;
                if(mtx == nullptr)
                {
                    done = visited.count(tn) != 0;
                } else
                {
                    std::lock_guard<std::mutex> lock(*mtx);
                    done = visited.count(t) != 0;
                }

                if(done || t == tn || cmp(t, tn) > dist) continue;
                if(mtx == nullptr)
                {
                    Graph<T>::add_edge(t, tn);
                } else
                {
                    std::lock_guard<std::mutex> lock(*mtx);
                    Graph<T>::add_edge(t, tn);
                }
            }

            if(mtx == nullptr)
            {
                visited.insert(t);
            } else
            {
                std::lock_guard<std::mutex> lock(*mtx);
                visited.insert(t);
            }
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
        void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            Graph<T>::add_node(c);
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            std::unordered_set<Node<T>*> updated_children;
            this->remove_edge(t1, t2);

            for(Node<T> *next1 : *n1)
            {
                updated_children.insert(next1);
            }

            for(Node<T> *next2 : *n2)
            {
                updated_children.insert(next2);
            }

            for(Node<T> *next : updated_children)
            {
                this->add_edge(c, next->get_value());
                to_update.emplace_back(c, next->get_value());
            }

            this->remove_node(t1);
            this->remove_node(t2);
        }
};

#endif