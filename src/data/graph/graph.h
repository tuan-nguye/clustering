#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <mutex>

#include "data/graph/node.h"
#include "data/graph/default_node.h"
#include "data/structures/maptor.h"

#ifndef __graph_include__
#define __graph_include__

/**
 * @brief Generic graph class that only uses the objects as keys. Nodes are
 * not publicly accessible.
 * 
 * @tparam T 
 */
template<typename T> class Graph
{
    private:
        std::unordered_map<T, Node<T>*> node_map;
        Maptor<T> elements;
        std::mutex mtx_count;
        long long edge_count = 0;
    protected:
        /**
         * @brief Get the node object
         * 
         * @param t 
         * @return Node<T>* 
         */
        Node<T>* get_node(T &t)
        {
            return node_map[t];
        }

        /**
         * @brief Create a node object
         * 
         * @param t 
         * @return Node<T>* 
         */
        virtual Node<T>* create_node(T &t)
        {
            Node<T> *node = new Default_Node<T>(t);
            return node;
        }
    public:
        /**
         * @brief add a node given an object t
         * 
         * @param t 
         */
        virtual void add_node(T &t)
        {
            if(find_node(t)) return;
            Node<T> *node = create_node(t);
            node_map[t] = node;
            elements.push_back(t);
        }

        /**
         * @brief remove a node given an object t, if it exists
         * 
         * @param t 
         */
        virtual void remove_node(T &t)
        {
            if(!find_node(t)) return;
            Node<T> *node = get_node(t);
            std::vector<T> children;
            get_children(children, t);
            
            for(T &tc : children)
            {
                remove_edge(t, tc);
            }

            elements.erase(t);
            node_map.erase(t);
            delete node;
        }

        /**
         * @brief find the node assigned to an object
         * 
         * @param t 
         * @return true if it exists
         * @return false if it doesn't exist in the graph
         */
        virtual bool find_node(T &t)
        {
            return elements.find(t);
        }

        /**
         * @brief add an undirected edge between the two nodes t1 and t2
         * 
         * @param t1 
         * @param t2 
         */
        virtual void add_edge(T &t1, T &t2)
        {
            add_edge_directed(t1, t2);
            add_edge_directed(t2, t1);
        }

        /**
         * @brief add a directed edge that goes from t1 to t2
         * 
         * @param t1 
         * @param t2 
         */
        virtual void add_edge_directed(T &t1, T &t2)
        {
            if(!find_node(t1) || !find_node(t2)) throw std::invalid_argument("t1 or t2 is not a node, can't add edge");
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->add_child(n2);
            std::lock_guard<std::mutex> lock(mtx_count);
            edge_count++;
        }

        /**
         * @brief remove all un/directed edges between t1 and t2
         * 
         * @param t1 
         * @param t2 
         */
        virtual void remove_edge(T &t1, T &t2)
        {
            remove_edge_directed(t1, t2);
            remove_edge_directed(t2, t1);
        }

        /**
         * @brief remove a directed edge from t1 to t2
         * 
         * @param t1 
         * @param t2 
         */
        virtual void remove_edge_directed(T &t1, T &t2)
        {
            if(!find_node(t1)) throw std::invalid_argument("t1 doesn't exist, can't remove edge");
            else if(!find_node(t2)) return;
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->remove_child(n2);
            std::lock_guard<std::mutex> lock(mtx_count);
            edge_count--;
        }

        /**
         * @brief Get the children objects
         * 
         * @param vec 
         * @param t 
         */
        virtual void get_children(std::vector<T>& vec, T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't get its children");
            Node<T> *n = get_node(t);
            vec.reserve(n->size());
            for(Node<T> *c : *n) vec.push_back(c->get_value());
        }

        /**
         * @brief get the last child from an object t, useful when the children nodes are sorted
         * 
         * @param t 
         * @return T& 
         */
        virtual T& last_child(T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't access last child");
            return get_node(t)->back()->get_value();
        }

        /**
         * @brief remove the last child of the node that belongs to t
         * 
         * @param t 
         */
        virtual void pop_back_child(T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't pop back child");
            get_node(t)->pop_back();
        }

        /**
         * @brief returns the number of children the node of t has
         * 
         * @param t 
         * @return int 
         */
        virtual int number_of_children(T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't return number of children");
            return get_node(t)->size();
        }

        /**
         * @brief returns a boolean value that indicates whether an object c is contained
         * as child of the object t
         * 
         * @param t 
         * @param c 
         * @return true 
         * @return false 
         */
        virtual bool is_child(T &t, T &c)
        {
            if(!find_node(t) || !find_node(c)) throw std::invalid_argument("t or c doesn't exist, can't return whether c is a child of t");
            return get_node(t)->contains_child(get_node(c));
        }

        /**
         * @brief G// get neigbhours of the node
         * no difference in bidirectional graph
         * in directional returns all nodes connected to it
         * 
         * @param vec 
         * @param t 
         */
        virtual void get_neighbours(std::vector<T> &vec, T &t)
        {
            get_children(vec, t);
        }

        /**
         * @brief returns the total number of nodes contained in the graph
         * 
         * @return int 
         */
        virtual int size() { return elements.size(); }

        /**
         * @brief returns the total number of edges contained in the graph
         * 
         * @return long long 
         */
        virtual long long size_edges() { return edge_count; }

        /**
         * @brief returns a maptor containing all nodes of the graph
         * 
         * @return Maptor<T>& 
         */
        virtual Maptor<T>& get_all_elements()
        {
            return elements;
        }

        /**
         * @brief delete all nodes and edges in the graph
         * 
         */
        virtual void clear()
        {
            elements.clear();
            for(auto &e : node_map)
            {
                Node<T> *n = e.second;
                delete n;
            }
            node_map.clear();
        }

        /**
         * @brief delete all edges between all nodes in the graph
         * 
         */
        virtual void clear_edges()
        {
            for(auto &e : node_map)
            {
                Node<T> *n = e.second;
                n->clear_children();
            }
            edge_count = 0;
        }

        /**
         * @brief print the graph's structure into the console
         * 
         */
        void print_structure()
        {
            std::string out = "graph[\n";

            for(auto &e : node_map)
            {
                out += e.first->to_string() + ": [";
                bool first = true;
                for(Node<T> *nc : *e.second)
                {
                    if(first) first = false;
                    else out += ", ";
                    out += nc->get_value()->to_string();
                }
                out += "]\n";
            }

            out += "]";

            std::cout << out << std::endl;
        }

        /**
         * @brief remove all edges of the graph and rebuild it entirely. each new edge is added to to_update
         * 
         * @param to_update 
         */
        virtual void rebuild(std::vector<std::pair<T, T>> &to_update) {}
};

#endif