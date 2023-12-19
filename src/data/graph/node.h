#include <vector>
#include <algorithm>
#include <iostream>

#include "data/structures/maptor.h"

#ifndef __graph_node_include__
#define __graph_node_include__

template <typename T> class Node
{
    private:
        // the value of the node
        T t;
    protected:
        /**
         * @brief provides an iterator to easily access its children nodes
         * 
         */
        class Node_Iterator
        {
            public:
                virtual Node<T>*& operator*() = 0;
                virtual Node_Iterator& operator++() = 0;
                virtual bool operator!=(const Node_Iterator& other) = 0;
        };

        /**
         * @brief Create a begin object of the node iterator
         * 
         * @return Node_Iterator* 
         */
        virtual Node_Iterator* create_begin() = 0;
        /**
         * @brief Create an end object of the node iterator
         * 
         * @return Node_Iterator* 
         */
        virtual Node_Iterator* create_end() = 0;
    public:
        /**
         * @brief a class that handles memory allocation of the node_iterators because
         * they are pointers for polymorphism reasons and can be varied. But returning
         * iterator pointers would make freeing memory complicated. So this iterator
         * wrapper is returned when asking for an iterator, instead.
         * 
         */
        class Iterator_Wrapper
        {
            private:
                Node_Iterator *n_it;
            public:
                Iterator_Wrapper(Node_Iterator *n_it): n_it(n_it) {}

                // change ownership of pointer and reset original iterator value
                Iterator_Wrapper(Iterator_Wrapper& other)
                {
                    other.n_it = n_it;
                    n_it = nullptr;
                }

                // free memory of derived iterator when destructed
                ~Iterator_Wrapper()
                {
                    if(n_it != nullptr)
                    {
                        delete n_it;
                    }
                }

                // dereference to access the node
                Node<T>*& operator*() { return **n_it; }

                // increment and then return current position
                Iterator_Wrapper& operator++()
                {
                    ++(*n_it);
                    return *this;
                }

                // inequality operator checking whether two iterator wrappers are the same
                bool operator!=(Iterator_Wrapper& other) { return *n_it != *other.n_it; }
        };

        typedef Iterator_Wrapper iterator;
    public:
        // create a new node with a value t
        Node(T t): t(t) {}
        // get value of the node
        T& get_value() { return t; }
        // remove a node from the set of children
        virtual void remove_child(Node<T> *n) = 0;
        // add a child node
        virtual void add_child(Node<T> *n) = 0;
        // return the last child
        virtual Node<T>*& back() = 0;
        // remove the last child
        virtual void pop_back() = 0;
        // returns true if the child is contained in the children set
        virtual bool contains_child(Node<T> *n) = 0;
        // remove all children
        virtual void clear_children() = 0;
        // returns the number of children
        virtual int size() = 0;

        // returns an iterator pointing at the first element
        iterator begin()
        {
            return Iterator_Wrapper(create_begin());
        }

        // returns an iterator pointing at the location behind the last element
        iterator end()
        {
            return Iterator_Wrapper(create_end());
        }
};

#endif