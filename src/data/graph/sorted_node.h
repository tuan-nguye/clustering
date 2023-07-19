#include <vector>
#include <algorithm>
#include <tuple>
#include <iostream>
#include <stdexcept>
#include <functional>

#include "data/graph/node.h"
#include "data/structures/sorted_vector.h"

#ifndef __Sorted_Node_include__
#define __Sorted_Node_include__

template <typename T> class Sorted_Node: public Node<T>
{
    private:
        std::function<float(T&, T&)> eval;
        Sorted_Vector<Node<T>*> children;

        std::function<float(Node<T>*&, Node<T>*&)> lambda()
        {
            T &node_val = this->get_value();
            std::function<float(T&, T&)> function = eval;

            std::function<float(Node<T>*&, Node<T>*&)> cmp = [&node_val, function](Node<T> *&n1, Node<T> *&n2) -> float
            {
                T &t1 = n1->get_value(), &t2 = n2->get_value();
                return function(node_val, t1) - function(node_val, t2);
            };
            
            return cmp;
        }
    protected:
        typedef typename Node<T>::Node_Iterator Node_Iterator;
        
        class Sorted_Node_Iterator: public Node_Iterator
        {
            private:
                typename std::vector<Node<T>*>::iterator it;
            public:
                Sorted_Node_Iterator(typename std::vector<Node<T>*>::iterator it): it(it) {}

                Node<T>*& operator*() { return *it; }
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
        Sorted_Node(T t, std::function<float(T&, T&)> eval): Node<T>(t), eval(eval), children(lambda()) {}

        void remove_child(Node<T> *n)
        {
            children.erase(n);
        }

        void add_child(Node<T> *n)
        {
            children.push(n);
        }

        Node<T>*& back() { return children.back(); }
        
        void pop_back() { children.pop_back(); }

        bool contains_child(Node<T> *n)
        {
            return children.contains(n);
        }
        
        void clear_children() { children.clear(); }

        int size() { return children.size(); }
};

#endif