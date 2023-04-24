#include "data/structures/maptor.h"

#ifndef __graph_node_include__
#define __graph_node_include__

template <typename T> class Node
{
    private:
        T t;
        Maptor<Node<T>*> children;
    public:
        Node(T t): t(t) {}
        T& get_value() { return t; }
        Maptor<Node<T>*>& get_children() { return children; }
};

#endif