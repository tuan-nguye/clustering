#include <vector>
#include <algorithm>
#include <tuple>

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
            int target_val = cmp(node_val, t, d);
            int l = 0, r = children.size();

            while(l <= r)
            {
                int m = (l+r)/2;
                float val_m = cmp(children[m]->get_value(), node_val, d);

                if(val_m == target_val) return m;
                else if(val_m < target_val) l = m+1;
                else r = m-1;
            }

            return l;
        }
    public:
        Sorted_Node(T t, float (*cmp)(T &t1, T &t2, float), float d): Node<T>(t), cmp(cmp), d(d) {}

        std::vector<Node<T>*>& get_children()
        {
            return children;
        }

        void remove_child(Node<T> *n)
        {
            T &t = n->get_value();
            int idx = binary_search(t);
            if(idx == children.size() || children[idx] != n) return;
            children.erase(children.begin()+idx);
        }

        void add_child(Node<T> *n)
        {
            int idx = binary_search(n->get_value());
            if(idx == children.size()) children.push_back(n);
            else children.insert(children.begin()+idx, n);
        }
        
        void clear_children() { children.clear(); }
};

#endif