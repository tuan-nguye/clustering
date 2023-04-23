#include <unordered_set>

#ifndef __graph_node_include__
#define __graph_node_include__

template <typename T> class Node
{
    private:
        T t;
        std::unordered_set<Node<T>*> children;
    public:
        Node(T t): t(t) {}
        T get_value() { return t; }
        std::unordered_set<Node<T>*>& get_children() { return children; }
};

#endif