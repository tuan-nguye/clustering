#include <vector>
#include <unordered_map>

#include "data/graph/node.h"
#include "data/structures/maptor.h"

#ifndef __graph_include__
#define __graph_include__

template<typename T> class Graph
{
    private:
        std::unordered_map<T, Node<T>*> node_map;
        Maptor<T> elements;
    protected:
        Node<T>* get_node(T &t)
        {
            return node_map[t];
        }
    public:
        void add_node(T &t)
        {
            Node<T> *node = new Node<T>(t);
            node_map[t] = node;
            elements.push_back(t);
        }

        void remove_node(T &t)
        {
            Node<T> *node = get_node(t);
            Maptor<Node<T>*> &children_set = node->get_children();
            std::vector<Node<T>*> children(children_set.begin(), children_set.end());

            for(Node<T> *next : children)
            {
                remove_edge(t, next->get_value());
            }

            delete node;
            elements.erase(t);
        }

        void add_edge(T &t1, T &t2)
        {
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->get_children().push_back(n2);
            n2->get_children().push_back(n1);
        }

        void remove_edge(T &t1, T &t2)
        {
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->get_children().erase(n2);
            n2->get_children().erase(n1);
        }

        void get_children(std::vector<T>& vec, T &t)
        {
            Maptor<Node<T>*>& children = get_node(t)->get_children();
            vec.reserve(children.size());
            for(Node<T> *c : children) vec.push_back(c->get_value());
        }

        Maptor<T>& get_all_values()
        {
            return elements;
        }

};

#endif