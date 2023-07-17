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

template<typename T> class Graph
{
    private:
        std::unordered_map<T, Node<T>*> node_map;
        Maptor<T> elements;
        std::mutex mtx_count;
        long long edge_count = 0;
    protected:
        Node<T>* get_node(T &t)
        {
            return node_map[t];
        }

        virtual Node<T>* create_node(T &t)
        {
            Node<T> *node = new Default_Node<T>(t);
            return node;
        }
    public:
        virtual void add_node(T &t)
        {
            if(find_node(t)) return;
            Node<T> *node = create_node(t);
            node_map[t] = node;
            elements.push_back(t);
        }

        virtual void remove_node(T &t)
        {
            if(!find_node(t)) return;
            Node<T> *node = get_node(t);
            std::vector<Node<T>*> children(node->get_children());
            
            for(Node<T> *c : children)
            {
                remove_edge(t, c->get_value());
            }

            elements.erase(t);
            node_map.erase(t);
            delete node;
        }

        virtual bool find_node(T &t)
        {
            return elements.find(t);
        }

        virtual void add_edge(T &t1, T &t2)
        {
            add_edge_directed(t1, t2);
            add_edge_directed(t2, t1);
        }

        virtual void add_edge_directed(T &t1, T &t2)
        {
            if(!find_node(t1) || !find_node(t2)) throw std::invalid_argument("t1 or t2 is not a node, can't add edge");
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->add_child(n2);
            std::lock_guard<std::mutex> lock(mtx_count);
            edge_count++;
        }

        virtual void remove_edge(T &t1, T &t2)
        {
            remove_edge_directed(t1, t2);
            remove_edge_directed(t2, t1);
        }

        virtual void remove_edge_directed(T &t1, T &t2)
        {
            if(!find_node(t1)) throw std::invalid_argument("t1 doesn't exist, can't remove edge");
            else if(!find_node(t2)) return;
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->remove_child(n2);
            std::lock_guard<std::mutex> lock(mtx_count);
            edge_count--;
        }

        // get children of the node
        virtual void get_children(std::vector<T>& vec, T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't get its children");
            std::vector<Node<T>*>& children = get_node(t)->get_children();
            vec.reserve(children.size());
            for(Node<T> *c : children) vec.push_back(c->get_value());
        }

        virtual T& last_child(T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't access last child");
            return get_node(t)->get_children().back()->get_value();
        }

        virtual void pop_back_child(T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't pop back child");
            get_node(t)->get_children().pop_back();
        }

        virtual int number_of_children(T &t)
        {
            if(!find_node(t)) throw std::invalid_argument("t doesn't exist, can't return number of children");
            return get_node(t)->get_children().size();
        }

        virtual bool is_child(T &t, T &c)
        {
            if(!find_node(t) || !find_node(c)) throw std::invalid_argument("t or c doesn't exist, can't return whether c is a child of t");
            return get_node(t)->contains_child(get_node(c));
        }

        // get neigbhours of the node
        // no difference in bidirectional graph
        // in directional returns all nodes connected to it
        virtual void get_neighbours(std::vector<T> &vec, T &t)
        {
            get_children(vec, t);
        }

        virtual int size() { return elements.size(); }

        virtual long long size_edges() { return edge_count; }

        virtual Maptor<T>& get_all_elements()
        {
            return elements;
        }

        virtual void clear()
        {
            elements.clear();
            for(auto &e : node_map)
            {
                Node<T> *n = e.second;
                delete n;
            }
        }

        virtual void clear_edges()
        {
            for(auto &e : node_map)
            {
                Node<T> *n = e.second;
                n->clear_children();
            }
            edge_count = 0;
        }

        void print_structure()
        {
            std::string out = "graph[\n";

            for(auto &e : node_map)
            {
                out += e.first->to_string() + ": [";
                bool first = true;
                for(Node<T> *nc : e.second->get_children())
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

        virtual void rebuild(std::vector<std::pair<T, T>> &to_update) {}
};

#endif