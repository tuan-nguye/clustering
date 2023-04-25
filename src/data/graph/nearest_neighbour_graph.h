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
            Maptor<T>& elements = get_all_elements();

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

        T& combine_node_to_from(T &t1, T &t2)
        {
            Node<T> *n1 = Graph<T>::get_node(t1), *n2 = Graph<T>::get_node(t2);

            for(Node<T> *next2 : n2->get_children())
            {
                if(n1 == next2) continue;
                Graph<T>::add_edge(n1->get_value(), next2->get_value());
            }

            remove_node(t2);
            return t1;
        }

        void get_children(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_children(vec, t);
        }

        int size() { return Graph<T>::size(); }

        Maptor<T>& get_all_elements()
        {
            return Graph<T>::get_all_elements();
        }
};

#endif