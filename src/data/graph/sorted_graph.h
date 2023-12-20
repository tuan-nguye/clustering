#include <functional>

#include "data/graph/graph.h"
#include "data/graph/sorted_node.h"

#ifndef __sorted_graph_include__
#define __sorted_graph_include__

/**
 * @brief Graph where the children of each node are sorted according
 * to a given distance function.
 * 
 * @tparam T 
 */
template<typename T> class Sorted_Graph: public Graph<T>
{
    private:
        // distance function
        std::function<float(T&, T&)> distance;
    protected:
        // create a new sorted node. factory method
        Node<T>* create_node(T &t)
        {
            return new Sorted_Node<T>(t, distance);
        }
    public:
        /**
         * @brief Construct a new Sorted_Graph object
         * 
         * @param distance distance function for comparing two nodes
         */
        Sorted_Graph(std::function<float(T&, T&)> distance): distance(distance) {}
};

#endif