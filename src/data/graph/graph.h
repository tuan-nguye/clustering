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
            std::vector<Node<T>*> &children = node->get_children();

            auto it = children.begin();
            while(it != children.end())
            {
                (*it)->remove_child(node);
                it = children.erase(it);
            }

            delete node;
            elements.erase(t);
        }

        void add_edge(T &t1, T &t2)
        {
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->add_child(n2);
            n2->add_child(n1);
        }

        void remove_edge(T &t1, T &t2)
        {
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->remove_child(n2);
            n2->remove_child(n1);
        }

        void get_children(std::vector<T>& vec, T &t)
        {
            std::vector<Node<T>*>& children = get_node(t)->get_children();
            vec.reserve(children.size());
            for(Node<T> *c : children) vec.push_back(c->get_value());
        }

        int size() { return elements.size(); }

        Maptor<T>& get_all_elements()
        {
            return elements;
        }

};

#endif