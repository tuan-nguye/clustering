#include <vector>
#include <algorithm>

#include "data/graph/node.h"

#ifndef __default_node_include__
#define __default_node_include__

template <typename T> class Default_Node: public Node<T>
{
    private:
        std::vector<Node<T>*> children;
    public:
        typedef typename Node<T>::Node_Iterator Node_Iterator;

        class Default_Node_Iterator: public Node_Iterator
        {
            private:
                typename std::vector<Node<T>*>::iterator it;
            public:
                Default_Node_Iterator(typename std::vector<Node<T>*>::iterator it): it(it) {}

                Node<T>*& operator*() { return *it; }
                Node_Iterator& operator++()
                {
                    ++it;
                    return *this;
                }
                bool operator!=(const Node_Iterator& other)
                {
                    Default_Node<T>::Default_Node_Iterator *sni = (Default_Node<T>::Default_Node_Iterator*) &other;
                    return it != sni->it;
                }
        };
        
        Node_Iterator* create_begin() { return new Default_Node<T>::Default_Node_Iterator(children.begin()); }
        Node_Iterator* create_end() { return new Default_Node<T>::Default_Node_Iterator(children.end()); }
    public:
        Default_Node(T t): Node<T>(t) {}
        void remove_child(Node<T> *n)
        {
            children.erase(std::remove(children.begin(), children.end(), n), children.end());
        }
        void add_child(Node<T> *n) { children.push_back(n); }
        Node<T>*& back() { return children.back(); };
        void pop_back() { children.pop_back(); };
        bool contains_child(Node<T> *n) { return std::find(children.begin(), children.end(), n) != children.end(); }
        void clear_children() { children.clear(); }
        int size() { return children.size(); }
};

#endif