#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "data/graph/sorted_node.h"
#include "data/structures/maptor.h"

#ifndef __knn_graph__
#define __knn_graph__


template<typename T> class KNN_Graph: public Auto_Edge_Graph<T>
{
    private:
        float (*cmp)(T&, T&, float);
        float dist;
        int k;
        std::unordered_map<Node<T>*, std::unordered_set<Node<T>*>> incoming;

        // add edge from t1 -> t2
        void add_edge_limit_k(T &t1, T &t2)
        {
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            std::vector<Node<T>*> &children1 = n1->get_children();
            
            if(children1.size() < k)
            {
                this->add_edge_directed(t1, t2);
            } else
            {
                Node<T> *last = children1.back();
                T &tl = last->get_value();
                if(cmp(t1, t2, dist) < cmp(t1, tl, dist))
                {
                    this->remove_edge_directed(t1, tl);
                    this->add_edge_directed(t1, t2);
                }
            }
            //std::cout << "k = " << children1.size() << std::endl;
        }

        void update_outgoing_edges()
        {

        }

        void update_incoming_edges()
        {

        }
    protected:
        Node<T>* create_node(T &t)
        {
            return new Sorted_Node<T>(t, cmp, dist);
        }

        void add_edge_on_condition(T &t1, T &t2, std::mutex &mtx)
        {
            // calculate score_diff, add when better than last element
            // if added, then cut size of children to k
            std::lock_guard<std::mutex> lock(mtx);
            add_edge_limit_k(t1, t2);
            add_edge_limit_k(t2, t1);
        }
    public:
        KNN_Graph(int k, float distance, float(*comparator)(T&, T&, float)): k(k), dist(distance), cmp(comparator) {}

        // 1. create new joined node
        // 2. iterate through all outgoing neighbours of t1, t2
        //    and add if c is <= b_limit
        // 3. if size is less than k, iterate through all nodes
        //    to find the rest
        // 4. iterate through all incoming neighbours of t1, t2,
        //    both will be deleted so replace them
        //    if possible add c, if not then iterate through all
        //    to find the one to replace it with

        void combine_nodes_into(T &c, T &t1, T &t2)
        {
            /*
            std::cout << "\nincoming" << std::endl;
            for(auto &e : incoming)
            {
                std::cout << e.first->get_value()->to_string() << ", " << e.second.size() << "children: ";
                for(Node<T> *n : e.second) std::cout << n->get_value()->to_string() << ", ";
                std::cout << std::endl;
            }
            */

            Graph<T>::add_node(c);
            Node<T> *nc = Graph<T>::get_node(c);
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            Maptor<T> outgoing;
            float b_limit = cmp(t1, n1->get_children().back()->get_value(), dist) + cmp(t2, n2->get_children().back()->get_value(), dist);
            /*
            std::cout << "join: t1 = " << t1->to_string() << " and t2 = " << t2->to_string() << std::endl;
            std::vector<T> inc1, inc2;
            for(Node<T> *n1 : incoming[n1]) inc1.push_back(n1->get_value());
            for(Node<T> *n2 : incoming[n2]) inc2.push_back(n2->get_value());
            std::cout << "incoming[n1] = ";
            print_collection(inc1.begin(), inc1.end());
            std::cout << "incoming[n2] = ";
            print_collection(inc2.begin(), inc2.end());
            std::vector<T> ch1, ch2;
            this->get_children(ch1, t1);
            this->get_children(ch2, t2);
            std::cout << "outgoing[n1] = ";
            print_collection(ch1.begin(), ch1.end());
            std::cout << "outgoing[n2] = ";
            print_collection(ch2.begin(), ch2.end());
            std::cout.flush();
            */

            // outgoing

            for(Node<T> *next1 : n1->get_children())
            {
                T &t = next1->get_value();
                incoming[next1].erase(n1);
                if(next1 == n2 || cmp(c, t, dist) > b_limit) continue;
                outgoing.push_back(t);
            }

            for(Node<T> *next2 : n2->get_children())
            {
                T &t = next2->get_value();
                incoming[next2].erase(n2);
                if(next2 == n1 || cmp(c, t, dist) > b_limit) continue;
                outgoing.push_back(t);
            }
            /*
            std::cout << "outgoing union\n";
            for(T &t : outgoing) std::cout << t->to_string() << ", ";
            std::cout << std::endl;
            */
            if(outgoing.size() >= k)
            {
                for(T &t : outgoing) this->add_edge_limit_k(c, t);
            } else
            {
                for(T &t : this->get_all_elements()) if(t != c && t != t1 && t != t2) this->add_edge_limit_k(c, t);
            }

            //std::cout << "joined children: " << nc->get_children().size() << std::endl;
            // incoming

            
            std::vector<Node<T>*> inc1_vec(incoming[n1].begin(), incoming[n1].end());
            for(Node<T> *n : inc1_vec)
            {
                if(n->get_children().size() == 0) continue;
                Node<T> *last = n->get_children().back();
                this->remove_edge_directed(n->get_value(), t1);
                if(n == n1 || n == n2 || n->get_value() == c) continue;
                
                if(cmp(n->get_value(), c, dist) <= cmp(n->get_value(), last->get_value(), dist) && !n->contains_child(nc))
                {
                    this->add_edge_limit_k(n->get_value(), c);
                } else
                {
                    Maptor<T> potential_children;
                    for(T &tnn : this->get_all_elements()) potential_children.push_back(tnn);
                    for(Node<T> *ncc : n->get_children())
                    {
                        potential_children.erase(ncc->get_value());
                    }
                    potential_children.erase(n->get_value());
                    potential_children.erase(t1);
                    potential_children.erase(t2);
                    potential_children.erase(c);
                    for(T tnn : potential_children) add_edge_limit_k(n->get_value(), tnn);
                }
            }

            
            std::vector<Node<T>*> inc2_vec(incoming[n2].begin(), incoming[n2].end());
            for(Node<T> *n : inc2_vec)
            {
                if(n->get_children().size() == 0) continue;
                Node<T> *last = n->get_children().back();
                this->remove_edge_directed(n->get_value(), t2);
                if(n == n1 || n == n2 || n->get_value() == c) continue;

                if(cmp(n->get_value(), c, dist) <= cmp(n->get_value(), last->get_value(), dist) && !n->contains_child(nc))
                {
                    this->add_edge_limit_k(n->get_value(), c);
                } else
                {
                    Maptor<T> potential_children;
                    for(T &tnn : this->get_all_elements()) potential_children.push_back(tnn);
                    for(Node<T> *ncc : n->get_children())
                    {
                        potential_children.erase(ncc->get_value());
                    }
                    potential_children.erase(n->get_value());
                    potential_children.erase(t1);
                    potential_children.erase(t2);
                    potential_children.erase(c);
                    for(T tnn : potential_children) add_edge_limit_k(n->get_value(), tnn);
                }
            }
            
            this->remove_edge(t1, t2);
            this->remove_node(t1);
            this->remove_node(t2);
            
            std::cout << "match: " << this->match_in_out() << std::endl;
            std::cout << "gone:\nt1 = " << gone(t1) << "\nt2 = " << gone(t2) << std::endl;
        }

        // override base function to update incoming map
        void remove_node(T &t)
        {
            incoming.erase(this->get_node(t));
            Graph<T>::remove_node(t);
        }

        // override base function to update incoming map
        void add_edge_directed(T &t1, T &t2)
        {
            incoming[this->get_node(t2)].insert(this->get_node(t1));
            Graph<T>::add_edge_directed(t1, t2);
        }

        // override base function to update incoming map
        void remove_edge_directed(T &t1, T &t2)
        {
            if(incoming.find(this->get_node(t2)) != incoming.end()) incoming[this->get_node(t2)].erase(this->get_node(t1));
            Graph<T>::remove_edge_directed(t1, t2);
        }

        bool match_in_out()
        {
            std::unordered_map<Node<T>*, std::vector<Node<T>*>> check;
            for(T &t : this->get_all_elements())
            {
                Node<T> *n = this->get_node(t);
                for(Node<T> *nn : n->get_children())
                {
                    check[nn].push_back(n);
                }
            }

            for(T &t : this->get_all_elements())
            {
                Node<T> *n = this->get_node(t);
                auto &children_check = check[n], &children_inc = incoming[n];
                if(children_check.size() != children_inc.size())
                {
                    std::cerr << "size doesn't match (check, inc) = (" + std::to_string(children_check.size()) + ", " + std::to_string(children_inc.size()) + "): ";
                    std::cerr << n->get_value()->to_string() << "\nchildren_check: ";
                    for(Node<T> *nc : children_check) std::cerr << nc->get_value()->to_string() << ", ";
                    std::cerr << "\nchildren_inc: ";
                    for(Node<T> *nc : children_inc) std::cerr << nc->get_value()->to_string() << ", ";
                    std::cerr << std::endl;
                    throw std::invalid_argument("");
                    return false;
                }

                Maptor<Node<T>*> diff;
                for(Node<T> *n : children_check) diff.push_back(n);
                for(Node<T> *n : children_inc) diff.erase(n);
                
                if(diff.size() != 0)
                {
                    std::cerr << "elements don't match: ";
                    std::cerr << n->get_value()->to_string() << "\nchildren_check: ";
                    for(Node<T> *nc : children_check) std::cerr << nc->get_value()->to_string() << ", ";
                    std::cerr << "\nchildren_inc";
                    for(Node<T> *nc : children_inc) std::cerr << nc->get_value()->to_string() << ", ";
                    std::cerr << std::endl;
                    throw std::invalid_argument("");
                    return false;
                }
            }

            return true;
        }

        bool gone(T &t)
        {
            // check outgoing edges
            for(T &tn : this->get_all_elements())
            {
                if(tn == t)
                {
                    throw std::invalid_argument(t->to_string() + " still exists in outgoing as key");
                    return false;
                }
                Node<T> *n = this->get_node(tn);
                for(Node<T> *nn : n->get_children())
                {
                    if(nn->get_value() == t)
                    {
                        throw std::invalid_argument(t->to_string() + " still exists in outgoing as child");
                        return false;
                    }
                }
            }

            // check incoming edges
            for(auto &e : incoming)
            {
                Node<T> *n = e.first;
                if(n->get_value() == t)
                {
                    throw std::invalid_argument(t->to_string() + " still exists in incoming as key");
                    return false;
                }

                for(Node<T> *nn : e.second)
                {
                    if(nn->get_value() == t)
                    {
                        throw std::invalid_argument(t->to_string() + " still exists in incoming as child");
                        return false;
                    }
                }
            }

            return true;
        }

        void print_collection(auto begin, auto end)
        {
            std::cout << "collection: ";
            auto it = begin;
            while(it != end)
            {
                std::cout << (*it)->to_string() << ", ";
                it++;
            }
            std::cout << std::endl;
        }
};

#endif