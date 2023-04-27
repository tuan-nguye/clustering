#include <vector>
#include <algorithm>

#include "data/structures/maptor.h"

#ifndef __graph_node_include__
#define __graph_node_include__

template <typename T> class Node
{
    private:
        T t;
        std::vector<Node<T>*> children;
    public:
        Node(T t): t(t) {}
        T& get_value() { return t; }
        std::vector<Node<T>*>& get_children() { return children; }
        void remove_child(Node<T> *n)
        {
            children.erase(std::remove(children.begin(), children.end(), n), children.end());
        }
        void add_child(Node<T> *n) { children.push_back(n); }
        void clear_children() { children.clear(); }
};

#endif