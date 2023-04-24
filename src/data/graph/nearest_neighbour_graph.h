#include <vector>

#include "data/graph/graph.h"

#ifndef __nearest_neigbour_graph_include__
#define __nearest_neigbour_graph_include__

template<typename T> class NN_Graph: protected Graph<T>
{
    private:
        float (*cmp)(T&, T&);
        float dist;
    public:
        NN_Graph(float distance, float(*comparator)(T&, T&)): dist(distance), cmp(comparator) {}
        
        void add_node(T &t)
        {
            Graph<T>::add_node(t);
            Maptor<T>& elements = get_all_values();

            for(T &elem : elements)
            {
                if(elem == t) continue;
                if(cmp(t, elem) <= dist)
                {
                    Graph<T>::add_edge(t, elem);
                }
            }
        }

        void remove_node(T &t)
        {
            Graph<T>::remove_node(t);
        }

        void get_children(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_children(vec, t);
        }

        Maptor<T>& get_all_values()
        {
            return Graph<T>::get_all_values();
        }
};

#endif