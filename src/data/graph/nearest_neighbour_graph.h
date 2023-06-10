#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>

#include "data/graph/graph.h"

extern int num_threads;

#ifndef __nearest_neigbour_graph_include__
#define __nearest_neigbour_graph_include__

template<typename T> class NN_Graph: protected Graph<T>
{
    private:
        float (*cmp)(T&, T&);
        float dist;
        
        void add_edges_parallel(T &t)
        {
            int size = get_all_elements().size();
            std::mutex mtx;
            std::vector<std::thread> threads;

            for(int i = 0; i < num_threads; i++)
            {
                int start = i*size/num_threads;
                int end = (i+1)*size/num_threads;
                if(start == end) continue;
                threads.emplace_back(&NN_Graph::add_edges, this, std::ref(t), std::ref(mtx), start, end);
            }

            for(std::thread &thread : threads)
            {
                thread.join();
            }
        }

        void add_edges(T &t, std::mutex &mtx, int start, int end)
        {
            Maptor<T> &elements = get_all_elements();

            for(int i = start; i < end; i++)
            {
                T &elem = elements[i];
                if(elem == t) continue;
                if(cmp(t, elem) <= dist)
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    Graph<T>::add_edge(t, elem);
                }
            }
        }
    public:
        NN_Graph(float distance, float(*comparator)(T&, T&)): dist(distance), cmp(comparator) {}
        
        void add_node(T &t)
        {
            Graph<T>::add_node(t);
            Maptor<T>& elements = get_all_elements();
            add_edges_parallel(t);
        }

        void remove_node(T &t)
        {
            Graph<T>::remove_node(t);
        }

        bool find(T &t)
        {
            return Graph<T>::find(t);
        }

        /*
        create new node from two other nodes
        children of new node is union of the children
        of the two others

        also remove reference to node by removing edge
        before removing the node, otherwise segfault
        */
        void combine_nodes_into(T &c, T &t1, T &t2)
        {
            Graph<T>::add_node(c);
            Node<T> *n1 = Graph<T>::get_node(t1), *n2 = Graph<T>::get_node(t2);
            std::unordered_set<Node<T>*> updated_children;
            Graph<T>::remove_edge(t1, t2);

            for(Node<T> *next1 : n1->get_children())
            {
                updated_children.insert(next1);
            }

            for(Node<T> *next2 : n2->get_children())
            {
                updated_children.insert(next2);
            }

            for(Node<T> *next : updated_children)
            {
                Graph<T>::add_edge(c, next->get_value());
            }

            remove_node(t1);
            remove_node(t2);
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