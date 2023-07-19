#include <vector>
#include <algorithm>
#include <iostream>

#include "data/structures/maptor.h"

#ifndef __graph_node_include__
#define __graph_node_include__

template <typename T> class Node
{
    private:
        T t;
    protected:
        class Node_Iterator
        {
            public:
                virtual Node<T>*& operator*() = 0;
                virtual Node_Iterator& operator++() = 0;
                virtual bool operator!=(const Node_Iterator& other) = 0;
        };

        virtual Node_Iterator* create_begin() = 0;
        virtual Node_Iterator* create_end() = 0;
    public:
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
                        //std::cout << "freed iterator" << std::endl;
                        delete n_it;
                    }
                }
                Node<T>*& operator*() { return **n_it; }
                Iterator_Wrapper& operator++()
                {
                    ++(*n_it);
                    return *this;
                }
                bool operator!=(Iterator_Wrapper& other) { return *n_it != *other.n_it; }
        };

        typedef Iterator_Wrapper iterator;
    public:
        Node(T t): t(t) {}
        T& get_value() { return t; }
        virtual void remove_child(Node<T> *n) = 0;
        virtual void add_child(Node<T> *n) = 0;
        virtual Node<T>*& back() = 0;
        virtual void pop_back() = 0;
        virtual bool contains_child(Node<T> *n) = 0;
        virtual void clear_children() = 0;
        virtual int size() = 0;

        iterator begin()
        {
            return Iterator_Wrapper(create_begin());
        }

        iterator end()
        {
            return Iterator_Wrapper(create_end());
        }
};

#endif