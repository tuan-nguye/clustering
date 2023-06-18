#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>

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
        std::unordered_map<Node<T>*, std::vector<Node<T>*>> incoming;

        // add edge from t1 -> t2
        void add_edge_limit_k(T &t1, T &t2)
        {
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            std::vector<Node<T>*> &children1 = n1->get_children();
            
            if(children1.size() == k)
            {
                Node<T> *last = children1.back();
                this->remove_edge_directed(t1, last->get_value());
                std::vector<Node<T>*> &incoming_last = incoming[last];
                incoming_last.erase(std::remove(incoming_last.begin(), incoming_last.end(), n1), incoming_last.end());
            }

            this->add_edge_directed(t1, t2);
            incoming[n2].push_back(n1);
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
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            std::vector<Node<T>*> &children1 = n1->get_children(), &children2 = n2->get_children();
            float score_diff = cmp(t1, t2, dist);

            if(children1.size() < k || score_diff < cmp(t1, children1.back()->get_value(), dist))
            {
                std::lock_guard<std::mutex> lock(mtx);
                add_edge_limit_k(t1, t2);
            }

            if(children2.size() < k || score_diff < cmp(t2, children2.back()->get_value(), dist))
            {
                std::lock_guard<std::mutex> lock(mtx);
                add_edge_limit_k(t2, t1);
            }
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
            std::cout << "\nincoming" << std::endl;
            for(auto &e : incoming)
            {
                std::cout << e.first->get_value()->to_string() << ", " << e.second.size() << "children: ";
                for(Node<T> *n : e.second) std::cout << n->get_value()->to_string() << ", ";
                std::cout << std::endl;
            }

            Graph<T>::add_node(c);
            Node<T> *nc = Graph<T>::get_node(c);
            Node<T> *n1 = this->get_node(t1), *n2 = this->get_node(t2);
            Maptor<T> outgoing;
            float b_limit = cmp(t1, n1->get_children().back()->get_value(), dist) + cmp(t2, n2->get_children().back()->get_value(), dist);

            // outgoing

            for(Node<T> *next1 : n1->get_children())
            {
                T &t = next1->get_value();
                if(next1 == n2 || cmp(c, t, dist) > b_limit) continue;
                outgoing.push_back(t);
                std::vector<Node<T>*> &incoming_next1 = incoming[next1];
                incoming_next1.erase(std::find(incoming_next1.begin(), incoming_next1.end(), n1), incoming_next1.end());
            }

            for(Node<T> *next2 : n2->get_children())
            {
                T &t = next2->get_value();
                if(next2 == n1 || cmp(c, t, dist) > b_limit) continue;
                outgoing.push_back(t);
                std::vector<Node<T>*> &incoming_next2 = incoming[next2];
                incoming_next2.erase(std::find(incoming_next2.begin(), incoming_next2.end(), n2), incoming_next2.end());
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

            incoming[n1].erase(std::remove(incoming[n1].begin(), incoming[n1].end(), n1), incoming[n1].end());
            incoming[n1].erase(std::remove(incoming[n1].begin(), incoming[n1].end(), n2), incoming[n1].end());
            incoming[n1].erase(std::remove(incoming[n1].begin(), incoming[n1].end(), nc), incoming[n1].end());
            for(Node<T> *n : incoming[n1])
            {
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
                    std::cout << "\nsize: " << potential_children.size() << std::endl;
                    for(Node<T> *ncc : n->get_children())
                    {
                        std::cout << "remove " << ncc->get_value()->to_string() << std::endl;
                        std::cout << "before:";
                        print_collection(potential_children.begin(), potential_children.end());
                        potential_children.erase(ncc->get_value());
                        std::cout << "after:";
                        print_collection(potential_children.begin(), potential_children.end());
                    }
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << n->get_value()->to_string() << std::endl;
                    potential_children.erase(n->get_value());
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << t1->to_string() << std::endl;
                    potential_children.erase(t1);
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << t2->to_string() << std::endl;
                    potential_children.erase(t2);
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << c->to_string() << std::endl;
                    std::cout << "find: " << potential_children.find(c) << std::endl;
                    //potential_children.erase(c);
                    std::cout << "size: " << potential_children.size() << std::endl;
                    for(T tnn : potential_children) add_edge_limit_k(n->get_value(), tnn);
                }
            }

            incoming[n2].erase(std::remove(incoming[n2].begin(), incoming[n2].end(), n1), incoming[n2].end());
            incoming[n2].erase(std::remove(incoming[n2].begin(), incoming[n2].end(), n2), incoming[n2].end());
            incoming[n2].erase(std::remove(incoming[n2].begin(), incoming[n2].end(), nc), incoming[n2].end());
            for(Node<T> *n : incoming[n2])
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
                    std::cout << "\nsize: " << potential_children.size() << std::endl;
                    for(Node<T> *ncc : n->get_children())
                    {
                        std::cout << "remove " << ncc->get_value()->to_string() << std::endl;
                        std::cout << "before:";
                        print_collection(potential_children.begin(), potential_children.end());
                        potential_children.erase(ncc->get_value());
                        std::cout << "after:";
                        print_collection(potential_children.begin(), potential_children.end());
                    }
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << n->get_value()->to_string() << std::endl;
                    potential_children.erase(n->get_value());
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << t1->to_string() << std::endl;
                    potential_children.erase(t1);
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << t2->to_string() << std::endl;
                    potential_children.erase(t2);
                    std::cout << "size: " << potential_children.size() << std::endl;
                    print_collection(potential_children.begin(), potential_children.end());
                    std::cout << "remove " << c->to_string() << std::endl;
                    std::cout << "find: " << potential_children.find(c) << std::endl;
                    potential_children.erase(t1);
                    std::cout << "size: " << potential_children.size() << std::endl;
                    for(T tnn : potential_children) add_edge_limit_k(n->get_value(), tnn);
                }
            }

            incoming.erase(n1);
            incoming.erase(n2);
            
            this->remove_edge(t1, t2);
            this->remove_node(t1);
            this->remove_node(t2);

            
            std::cout << "check: " << this->match_in_out() << std::endl;
            std::cout << "gone:\nt1 = " << gone(t1) << "\nt2 = " << gone(t2) << std::endl;
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
                std::vector<Node<T>*> &children_check = check[n], &children_inc = incoming[n];
                if(children_check.size() != children_inc.size())
                {
                    std::cerr << "size doesn't match: ";
                    std::cerr << n->get_value()->to_string() << "\nchildren_check: ";
                    for(Node<T> *nc : children_check) std::cerr << nc->get_value()->to_string() << ", ";
                    std::cerr << "\nchildren_inc";
                    for(Node<T> *nc : children_inc) std::cerr << nc->get_value()->to_string() << ", ";
                    std::cerr << std::endl;
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
                    return false;
                }
            }

            return true;
        }

        bool gone(T &t)
        {
            for(T &tn : this->get_all_elements())
            {
                if(tn == t)
                {
                    std::cerr << t->to_string() << " still exists" << std::endl;
                    return false;
                }
                Node<T> *n = this->get_node(tn);
                for(Node<T> *nn : n->get_children())
                {
                    if(nn->get_value() == t)
                    {
                        std::cerr << t->to_string() << " still exists" << std::endl;
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