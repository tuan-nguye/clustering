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

        void add_edges_single()
        {
            for(T &t : this->get_all_elements())
            {
                add_edges_operation(t, nullptr);
            }
        }

        void add_edges_parallel()
        {
            int size = this->size();
            std::mutex mtx;
            std::vector<std::thread> threads;

            for(int i = 0; i < num_threads; i++)
            {
                int start = i*size/num_threads;
                int end = (i+1)*size/num_threads;
                if(start == end) continue;
                threads.emplace_back(&Auto_Edge_Graph::add_edges_parallel_thread, this, &mtx, start, end);
            }

            for(std::thread &thread : threads)
            {
                thread.join();
            }
        }

        void add_edges_parallel_thread(std::mutex *mtx, int start, int end)
        {
            Maptor<T> &elements = this->get_all_elements();

            for(int i = start; i < end; i++)
            {
                add_edges_operation(elements[i], mtx);
            }
        }
    protected:
        virtual void add_edges_operation(T &t, std::mutex *mtx) = 0;

        void add_node(T &t)
        {
            Graph<T>::add_node(t);
        }

        virtual void build_graph()
        {
            if(parallel) add_edges_parallel();
            else add_edges_single();
        }
    public:        
        virtual void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update) = 0;

        // adds all nodes from values as nodes
        // generate edges automatically either parallel or single threading
        void set_nodes(std::vector<T> &values)
        {
            this->clear();
            for(T &v : values) add_node(v);
            build_graph();
        }

        void remove_node(T &t)
        {
            Graph<T>::remove_node(t);
        }

        bool find_node(T &t)
        {
            return Graph<T>::find_node(t);
        }

        void get_children(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_children(vec, t);
        }

        T& last_child(T &t)
        {
            return Graph<T>::last_child(t);
        }

        void pop_back_child(T &t)
        {
            Graph<T>::pop_back_child(t);
        }
        
        int number_of_children(T &t)
        {
            return Graph<T>::number_of_children(t);
        }

        bool is_child(T &t, T &c)
        {
            return Graph<T>::is_child(t, c);
        }

        virtual void get_neighbours(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_neighbours(vec, t);
        }

        int size() { return Graph<T>::size(); }

        long long size_edges() { return Graph<T>::size_edges(); }

        Maptor<T>& get_all_elements()
        {
            return Graph<T>::get_all_elements();
        }

        virtual void clear() { Graph<T>::clear(); }

        virtual void clear_edges() { Graph<T>::clear_edges(); }

        void set_parallel(bool parallel) { this->parallel = parallel; }

        bool get_parallel() { return parallel; }

        virtual void rebuild(std::vector<std::pair<T, T>> &to_update) {}

        void print_structure() { Graph<T>::print_structure(); }
};

#endif