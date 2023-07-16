#include <functional>

#include "data/graph/graph.h"
#include "data/graph/sorted_node.h"

#ifndef __sorted_graph_include__
#define __sorted_graph_include__

template<typename T> class Sorted_Graph: public Graph<T>
{
    private:
        std::function<float(T&, T&)> distance;
    protected:
        Node<T>* create_node(T &t)
        {
            return new Sorted_Node<T>(t, distance);
        }
    public:
        Sorted_Graph(std::function<float(T&, T&)> distance): distance(distance) {}
};

#endif