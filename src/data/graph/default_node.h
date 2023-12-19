#include <vector>
#include <algorithm>

#include "data/graph/node.h"

#ifndef __default_node_include__
#define __default_node_include__

/**
 * @brief default node class that is used by graphs
 * 
 * @tparam T 
 */
template <typename T> class Default_Node: public Node<T>
{
    private:
        /**
         * @brief vector that contains the children nodes
         * 
         */
        std::vector<Node<T>*> children;
    public:
        typedef typename Node<T>::Node_Iterator Node_Iterator;


        /**
         * @brief iterator class for the default node
         * 
         */
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
        /**
         * @brief Create a begin object of the node iterator
         * 
         * @return Node_Iterator* 
         */
        Node_Iterator* create_begin() { return new Default_Node<T>::Default_Node_Iterator(children.begin()); }
        /**
         * @brief Create an end object of the node iterator
         * 
         * @return Node_Iterator* 
         */
        Node_Iterator* create_end() { return new Default_Node<T>::Default_Node_Iterator(children.end()); }
    public:
        // constructor of the default node class. Requires a value 
        Default_Node(T t): Node<T>(t) {}

        // remove a node from the set of children
        void remove_child(Node<T> *n)
        {
            children.erase(std::remove(children.begin(), children.end(), n), children.end());
        }

        // add a child node
        void add_child(Node<T> *n) { children.push_back(n); }

        // return the last child
        Node<T>*& back() { return children.back(); };

        // remove the last child
        void pop_back() { children.pop_back(); };

        // returns true if the child is contained in the children set
        bool contains_child(Node<T> *n) { return std::find(children.begin(), children.end(), n) != children.end(); }
        
        // remove all children
        void clear_children() { children.clear(); }

        // returns the number of children
        int size() { return children.size(); }
};

#endif