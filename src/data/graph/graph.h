#include <vector>
#include <unordered_map>

#include "data/graph/node.h"

#ifndef __cluster_node_include__
#define __cluster_node_include__

template <typename T> class Graph
{
    private:
        std::unordered_map<T*, Node<T>*> node_map;
        Node<T>* get_node(T *t);
    public:
        void add_node(T *t);
        void remove_node(T *t);
        void add_edge(T *t1, T *t2);
        void remove_edge(T *t1, T *t2);
        void get_all_values(std::vector<T*> &vec);
};

#endif