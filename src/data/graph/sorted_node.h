#include <vector>
#include <algorithm>
#include <tuple>
#include <iostream>
#include <stdexcept>

#include "data/graph/node.h"

#ifndef __Sorted_Node_include__
#define __Sorted_Node_include__

template <typename T> class Sorted_Node: public Node<T>
{
    private:
        float (*cmp)(T &t1, T &t2, float);
        float d;
        std::vector<Node<T>*> children;

        // returns the index of the element if it exists
        // otherwise the idx of the first larger element
        int binary_search(T &t)
        {
            if(children.empty()) return 0;
             
            T &node_val = this->get_value();
            float target_val = cmp(node_val, t, d);
            int l = 0, r = children.size()-1;

            while(l <= r)
            {
                int m = (l+r)/2;
                //std::cout << "(l, m, r, size) = " << "(" << l << ", " << m << ", " << r << ", " << children.size() << ")" << std::endl;
                T &tm = children[m]->get_value();
                if(tm == t) return m;

                float val_m = cmp(node_val, tm, d);
                if(val_m == target_val)
                {
                    int idx_lin = linear_search(t);
                    return idx_lin != children.size() ? idx_lin : m;
                } else if(val_m < target_val)
                {
                    l = m+1;
                } else
                {
                    r = m-1;
                }
            }
            
            return l;
        }

        int linear_search(T &t)
        {
            int idx = 0;

            while(idx < children.size())
            {
                if(children[idx]->get_value() == t) break;
                idx++;
            }

            return idx;
        }
    public:
        Sorted_Node(T t, float (*cmp)(T &t1, T &t2, float), float d): Node<T>(t), cmp(cmp), d(d) {}

        std::vector<Node<T>*>& get_children()
        {
            return children;
        }

        void remove_child(Node<T> *n)
        {
            int k_before = children.size();
            T &t = n->get_value();
            int idx = binary_search(t);
            //int idx = linear_search(t);
            if(idx == children.size() || children[idx] != n) return;
            children.erase(children.begin()+idx);
        }

        void add_child(Node<T> *n)
        {
            int idx = binary_search(n->get_value());
            if(idx == children.size()) children.push_back(n);
            else children.insert(children.begin()+idx, n);
        }

        bool contains_child(Node<T> *n)
        {
            int idx = binary_search(n->get_value());
            return idx != children.size() && children[idx] == n;
        }
        
        void clear_children() { children.clear(); }
};

#endif