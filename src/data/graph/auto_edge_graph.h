#include <mutex>
#include <thread>
#include "data/graph/graph.h"

extern int num_threads;

#ifndef __auto_edge_graph_include__
#define __auto_edge_graph_include__

template<typename T> class Auto_Edge_Graph: protected Graph<T>
{
    private:
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
                threads.emplace_back(&Auto_Edge_Graph::add_edges, this, std::ref(t), std::ref(mtx), start, end);
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
                add_edge_on_condition(t, elem, mtx);
            }
        }
    protected:
        virtual void add_edge_on_condition(T &t1, T &t2, std::mutex &mtx) = 0;
    public:        
        virtual void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update) = 0;

        void add_node(T &t)
        {
            Graph<T>::add_node(t);
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

        void get_children(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_children(vec, t);
        }

        virtual void get_neighbours(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_neighbours(vec, t);
        }

        int size() { return Graph<T>::size(); }

        Maptor<T>& get_all_elements()
        {
            return Graph<T>::get_all_elements();
        }
};

#endif