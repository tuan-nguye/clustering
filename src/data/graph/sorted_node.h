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
        std::function<float(T&, T&)> func;
        Sorted_Vector<Node<T>*> children;

        std::function<float(Node<T>*&, Node<T>*&)> lambda()
        {
            T &node_val = this->get_value();
            std::function<float(T&, T&)> function = func;

            std::function<float(Node<T>*&, Node<T>*&)> cmp = [&node_val, function](Node<T> *&n1, Node<T> *&n2) -> float
            {
                T &t1 = n1->get_value(), &t2 = n2->get_value();
                return function(node_val, t1) - function(node_val, t2);
            };
            
            return cmp;
        }

    public:
        Sorted_Node(T t, std::function<float(T&, T&)> func): Node<T>(t), func(func), children(lambda()) {}

        std::vector<Node<T>*>& get_children()
        {
            return children.get_vector();
        }

        void remove_child(Node<T> *n)
        {
            children.erase(n);
        }

        void add_child(Node<T> *n)
        {
            children.push(n);
        }

        bool contains_child(Node<T> *n)
        {
            return children.contains(n);
        }
        
        void clear_children() { children.clear(); }
};

#endif