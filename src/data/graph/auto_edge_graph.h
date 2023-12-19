#include <mutex>
#include <thread>
#include "data/graph/graph.h"

extern int num_threads;

#ifndef __auto_edge_graph_include__
#define __auto_edge_graph_include__

/**
 * @brief Graph class where edges cannot be added manually, but are defined via
 * a condition which adds the edges automatically, if it's fulfilled.
 * 
 * @tparam T 
 */
template<typename T> class Auto_Edge_Graph: protected Graph<T>
{
    private:
        bool parallel = false;

        /**
         * @brief check condition and add all edges for all keys. single threaded
         * 
         */
        void add_edges_single()
        {
            for(T &t : this->get_all_elements())
            {
                add_edges_operation(t, nullptr);
            }
        }

        /**
         * @brief check condition and add all edges for all keys. multi threaded
         * 
         */
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

        /**
         * @brief function that is called for each thread
         * 
         * @param mtx 
         * @param start 
         * @param end 
         */
        void add_edges_parallel_thread(std::mutex *mtx, int start, int end)
        {
            Maptor<T> &elements = this->get_all_elements();

            for(int i = start; i < end; i++)
            {
                add_edges_operation(elements[i], mtx);
            }
        }
    protected:
        /**
         * @brief iterate through all keys and add edge if a certain condition is fulfilled
         * 
         * @param t 
         * @param mtx 
         */
        virtual void add_edges_operation(T &t, std::mutex *mtx) = 0;

        /**
         * @brief add node to the graph
         * 
         * @param t 
         */
        void add_node(T &t)
        {
            Graph<T>::add_node(t);
        }

        /**
         * @brief Go through all the nodes in the graph and add edges accordingly.
         * 
         */
        virtual void build_graph()
        {
            if(parallel) add_edges_parallel();
            else add_edges_single();
        }
    public:
        /**
         * @brief join to nodes into one. edges of the original nodes could be reused for the 
         * new node.
         * 
         * @param c 
         * @param t1 
         * @param t2 
         * @param to_update 
         */
        virtual void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update) = 0;
 
        /**
         * @brief adds all nodes from values as nodes
         * generate edges automatically either parallel or single threading
         * 
         * @param values 
         */
        void set_nodes(std::vector<T> &values)
        {
            this->clear();
            for(T &v : values) add_node(v);
            build_graph();
        }

        /**
         * @brief remove the node from the graph
         * 
         * @param t 
         */
        void remove_node(T &t)
        {
            Graph<T>::remove_node(t);
        }

        /**
         * @brief checks whether a node exists in the graph
         * 
         * @param t 
         * @return true 
         * @return false 
         */
        bool find_node(T &t)
        {
            return Graph<T>::find_node(t);
        }

        /**
         * @brief Get the children objects of t and save them in the given vector
         * 
         * @param vec 
         * @param t 
         */
        void get_children(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_children(vec, t);
        }

        /**
         * @brief return the last child of the key t
         * 
         * @param t 
         * @return T& 
         */
        T& last_child(T &t)
        {
            return Graph<T>::last_child(t);
        }

        /**
         * @brief remove the last child of the node assigned to t
         * 
         * @param t 
         */
        void pop_back_child(T &t)
        {
            Graph<T>::pop_back_child(t);
        }
        
        /**
         * @brief returns the number of children of the object t
         * 
         * @param t 
         * @return int 
         */
        int number_of_children(T &t)
        {
            return Graph<T>::number_of_children(t);
        }

        /**
         * @brief returns true if c is a child of t
         * 
         * @param t 
         * @param c 
         * @return true 
         * @return false 
         */
        bool is_child(T &t, T &c)
        {
            return Graph<T>::is_child(t, c);
        }

        /**
         * @brief get the neighbours of t and save them in vec
         * 
         * @param vec 
         * @param t 
         */
        virtual void get_neighbours(std::vector<T> &vec, T &t)
        {
            Graph<T>::get_neighbours(vec, t);
        }

        /**
         * @brief returns the number of nodes in the graph
         * 
         * @return int 
         */
        int size() { return Graph<T>::size(); }

        /**
         * @brief returns the number of edges
         * 
         * @return long long 
         */
        long long size_edges() { return Graph<T>::size_edges(); }

        /**
         * @brief Get the all elements contained in the graph
         * 
         * @return Maptor<T>& 
         */
        Maptor<T>& get_all_elements()
        {
            return Graph<T>::get_all_elements();
        }

        /**
         * @brief clear all nodes and edges
         * 
         */
        virtual void clear() { Graph<T>::clear(); }

        /**
         * @brief clear all edges
         * 
         */
        virtual void clear_edges() { Graph<T>::clear_edges(); }

        /**
         * @brief Set whether parallelization should be used to build the graph
         * 
         * @param parallel 
         */
        void set_parallel(bool parallel) { this->parallel = parallel; }

        /**
         * @brief returns whether parallelization is enabled
         * 
         * @return true 
         * @return false 
         */
        bool get_parallel() { return parallel; }

        /**
         * @brief rebuild the graph by adding edges and storing them in to_update
         * to indicate how the graph structure changed
         * 
         * @param to_update 
         */
        virtual void rebuild(std::vector<std::pair<T, T>> &to_update) {}

        /**
         * @brief print the graph structure
         * 
         */
        void print_structure() { Graph<T>::print_structure(); }
};

#endif