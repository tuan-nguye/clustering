#include <vector>
#include <algorithm>

#include "data/graph/node.h"

#ifndef __default_node_include__
#define __default_node_include__

template <typename T> class Default_Node: public Node<T>
{
    private:
        std::vector<Node<T>*> children;
    public:
        Default_Node(T t): Node<T>(t) {}
        std::vector<Node<T>*>& get_children() { return children; }
        void remove_child(Node<T> *n)
        {
            children.erase(std::remove(children.begin(), children.end(), n), children.end());
        }
        void add_child(Node<T> *n) { children.push_back(n); }
        bool contains_child(Node<T> *n) { return std::find(children.begin(), children.end(), n) != children.end(); }
        void clear_children() { children.clear(); }
};

#endif