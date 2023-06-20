#include <vector>
#include <unordered_map>
#include <iostream>

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
            Node<T> *node = create_node(t);
            node_map[t] = node;
            elements.push_back(t);
        }

        virtual void remove_node(T &t)
        {
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

        virtual bool find(T &t)
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
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->add_child(n2);
            edge_count++;
        }

        virtual void remove_edge(T &t1, T &t2)
        {
            remove_edge_directed(t1, t2);
            remove_edge_directed(t2, t1);
        }

        virtual void remove_edge_directed(T &t1, T &t2)
        {
            Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
            n1->remove_child(n2);
            edge_count--;
        }

        virtual void get_children(std::vector<T>& vec, T &t)
        {
            std::vector<Node<T>*>& children = get_node(t)->get_children();
            vec.reserve(children.size());
            for(Node<T> *c : children) vec.push_back(c->get_value());
        }

        virtual int size() { return elements.size(); }

        virtual long long size_edges() { return edge_count; }

        virtual Maptor<T>& get_all_elements()
        {
            return elements;
        }

};

#endif