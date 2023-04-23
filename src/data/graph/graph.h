#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "data/graph/node.h"

#ifndef __cluster_node_include__
#define __cluster_node_include__

template<typename T> class Graph
{
    private:
        std::unordered_map<T, Node<T>*> node_map;
        Node<T>* get_node(T t)
        {
            return node_map[t];
        }
    public:
        void add_node(T t)
        {
            Node<T> *node = new Node<T>(t);
            node_map[t] = node;
        }

        void remove_node(T t)
        {
            Node<T> *node = get_node(t);
            std::unordered_set<Node<T>*> children_set = node->get_children();
            std::vector<Node<T>*> children(children_set.begin(), children_set.end());

            for(Node<T> *next : children)
            {
                remove_edge(t, next->get_value());
            }
        }

        void add_edge(T t1, T t2)
        {
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->get_children().insert(n2);
            n2->get_children().insert(n1);
        }

        void remove_edge(T t1, T t2)
        {
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->get_children().erase(n2);
            n2->get_children().erase(n1);
        }

        void get_children(std::vector<T>& vec, T t)
        {
            std::unordered_set<Node<T>*>& children = get_node(t)->get_children();
            vec.reserve(children.size());
            for(Node<T> *c : children) vec.push_back(c->get_value());
        }

        void get_all_values(std::vector<T> &vec)
        {
            vec.reserve(node_map.size());

            for(auto &entry : node_map)
            {
                vec.push_back(entry.second);
            }
        }

};

#endif