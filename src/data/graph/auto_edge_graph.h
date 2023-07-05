#include <mutex>
#include <thread>
#include "data/graph/graph.h"

extern int num_threads;

#ifndef __auto_edge_graph_include__
#define __auto_edge_graph_include__

template<typename T> class Auto_Edge_Graph: protected Graph<T>
{
    private:
        bool parallel = false;

        void add_edges_single(std::vector<T> &values)
        {
            for(T &t : values)
            {
                add_edges_operation(t, nullptr);
            }
        }

        void add_edges_parallel(std::vector<T> &values)
        {
            int size = values.size();
            std::mutex mtx;
            std::vector<std::thread> threads;

            for(int i = 0; i < num_threads; i++)
            {
                int start = i*size/num_threads;
                int end = (i+1)*size/num_threads;
                if(start == end) continue;
                threads.emplace_back(&Auto_Edge_Graph::add_edges_parallel_thread, this, std::ref(values), &mtx, start, end);
            }

            for(std::thread &thread : threads)
            {
                thread.join();
            }
        }

        void add_edges_parallel_thread(std::vector<T> &values, std::mutex *mtx, int start, int end)
        {
            for(int i = start; i < end; i++)
            {
                add_edges_operation(values[i], mtx);
            }
        }
    protected:
        virtual void add_edges_operation(T &t, std::mutex *mtx) = 0;
        void add_node(T &t)
        {
            Graph<T>::add_node(t);
        }
    public:        
        virtual void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update) = 0;

        // adds all nodes from values as nodes
        // generate edges automatically either parallel or single threading
        void set_nodes(std::vector<T> &values)
        {
            for(T &v : values) add_node(v);
            if(parallel) add_edges_parallel(values);
            else add_edges_single(values);
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

        void set_parallel(bool parallel) { this->parallel = parallel; }
};

#endif