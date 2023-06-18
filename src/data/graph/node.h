#include <vector>
#include <algorithm>

#include "data/structures/maptor.h"

#ifndef __graph_node_include__
#define __graph_node_include__

template <typename T> class Node
{
    private:
        T t;
    public:
        Node(T t): t(t) {}
        T& get_value() { return t; }
        virtual std::vector<Node<T>*>& get_children() = 0;
        virtual void remove_child(Node<T> *n) = 0;
        virtual void add_child(Node<T> *n) = 0;
        virtual bool contains_child(Node<T> *n) = 0;
        virtual void clear_children() = 0;
};

#endif