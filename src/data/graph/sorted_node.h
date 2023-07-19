#include <vector>
#include <algorithm>
#include <tuple>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <utility>

#include "data/graph/node.h"
#include "data/structures/sorted_vector.h"

#ifndef __Sorted_Node_include__
#define __Sorted_Node_include__

template <typename T> class Sorted_Node: public Node<T>
{
    private:
        typedef std::pair<float, Node<T>*> pair;
        std::function<float(T&, T&)> eval;
        Sorted_Vector<pair> children;

        static float compare(pair &p1, pair &p2)
        {
            return p1.first - p2.first;
        }

        pair get_pair(Node<T> *n)
        {
            T &node_val = this->get_value(), &new_val = n->get_value();
            return std::make_pair(eval(node_val, new_val), n);
        }
    protected:
        typedef typename Node<T>::Node_Iterator Node_Iterator;
        
        class Sorted_Node_Iterator: public Node_Iterator
        {
            private:
                typename std::vector<pair>::iterator it;
            public:
                Sorted_Node_Iterator(typename std::vector<pair>::iterator it): it(it) {}

                Node<T>*& operator*() { return (*it).second; }
                Node_Iterator& operator++()
                {
                    ++it;
                    return *this;
                }
                bool operator!=(const Node_Iterator& other)
                {
                    Sorted_Node<T>::Sorted_Node_Iterator *sni = (Sorted_Node<T>::Sorted_Node_Iterator*) &other;
                    return it != sni->it;
                }
        };
        
        Node_Iterator* create_begin() { return new Sorted_Node<T>::Sorted_Node_Iterator(children.begin()); }
        Node_Iterator* create_end() { return new Sorted_Node<T>::Sorted_Node_Iterator(children.end()); }
    public:
        Sorted_Node(T t, std::function<float(T&, T&)> eval): Node<T>(t), eval(eval), children(compare) {}

        void remove_child(Node<T> *n)
        {
            children.erase(get_pair(n));
        }

        void add_child(Node<T> *n)
        {
            pair pn = get_pair(n);
            children.push(pn);
        }

        Node<T>*& back() { return children.back().second; }
        
        void pop_back() { children.pop_back(); }

        bool contains_child(Node<T> *n)
        {
            return children.contains(get_pair(n));
        }
        
        void clear_children() { children.clear(); }

        int size() { return children.size(); }
};

#endif