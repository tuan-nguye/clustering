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

/**
 * @brief a node where the children are sorted according to a given evaluation
 * function
 * 
 * @tparam T 
 */
template <typename T> class Sorted_Node: public Node<T>
{
    private:
        // a pair for saving the evaluation value so it doesn't need to be recalculated
        // and the child node
        typedef std::pair<float, Node<T>*> pair;
        // evaluation function that returns a float indicating the distance between two objects t1 and t2
        std::function<float(T&, T&)> eval;
        // sorted children container
        Sorted_Vector<pair> children;

        // compare two pairs for sorting
        static float compare(pair &p1, pair &p2)
        {
            return p1.first - p2.first;
        }

        // get pair of the given node
        pair get_pair(Node<T> *n)
        {
            T &node_val = this->get_value(), &new_val = n->get_value();
            return std::make_pair(eval(node_val, new_val), n);
        }
    protected:
        typedef typename Node<T>::Node_Iterator Node_Iterator;
        
        /**
         * @brief iterator class for the sorted node.
         * 
         */
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
        /**
         * @brief Create a begin object of the node iterator
         * 
         * @return Node_Iterator* 
         */
        Node_Iterator* create_begin() { return new Sorted_Node<T>::Sorted_Node_Iterator(children.begin()); }
        /**
         * @brief Create an end object of the node iterator
         * 
         * @return Node_Iterator* 
         */
        Node_Iterator* create_end() { return new Sorted_Node<T>::Sorted_Node_Iterator(children.end()); }
    public:
        // create a new sorted node with a value t and an evaluation function
        // which indicates how to sort the children
        Sorted_Node(T t, std::function<float(T&, T&)> eval): Node<T>(t), eval(eval), children(compare) {}

        // remove a node from the set of children
        void remove_child(Node<T> *n)
        {
            children.erase(get_pair(n));
        }

        // add a child node
        void add_child(Node<T> *n)
        {
            pair pn = get_pair(n);
            children.push(pn);
        }

        // return the last child
        Node<T>*& back() { return children.back().second; }
        
        // remove the last child
        void pop_back() { children.pop_back(); }

        // returns true if the child is contained in the children set
        bool contains_child(Node<T> *n)
        {
            return children.contains(get_pair(n));
        }
        
        // remove all children
        void clear_children() { children.clear(); }

        // returns the number of children
        int size() { return children.size(); }
};

#endif