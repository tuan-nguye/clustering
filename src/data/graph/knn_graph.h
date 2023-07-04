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
        std::function<float(T&, T&)> cmp;
        
        float dist;
        int k;
        std::unordered_map<T, std::unordered_set<T>> incoming;

        // add edge from t1 -> t2
        void add_edge_limit_k(T &t1, T &t2)
        {
            std::vector<T> children1;
            this->get_children(children1, t1);
            
            if(children1.size() < k)
            {
                this->add_edge_directed(t1, t2);
            } else
            {
                T &tl = children1.back();
                if(cmp(t1, t2) < cmp(t1, tl))
                {
                    this->remove_edge_directed(t1, tl);
                    this->add_edge_directed(t1, t2);
                }
            }
            //std::cout << "k = " << children1.size() << std::endl;
        }

        void add_edge_limit_k_and_update(T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            add_edge_limit_k(t1, t2);
            to_update.emplace_back(t1, t2);
        }

        void update_outgoing_edges(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::vector<T> children1, children2;
            this->get_children(children1, t1);
            this->get_children(children2, t2);
            Maptor<T> outgoing;
            float b_limit = cmp(t1, children1.back()) + cmp(t2, children2.back());
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

            for(T &tn1 : children1)
            {
                incoming[tn1].erase(t1);
                if(cmp(c, tn1) > b_limit) continue;
                outgoing.push_back(tn1);
            }

            for(T &tn2 : children2)
            {
                incoming[tn2].erase(t2);
                if(cmp(c, tn2) > b_limit) continue;
                outgoing.push_back(tn2);
            }
            outgoing.erase(t1);
            outgoing.erase(t2);
            /*
            std::cout << "outgoing union\n";
            for(T &t : outgoing) std::cout << t->to_string() << ", ";
            std::cout << std::endl;
            */
            if(outgoing.size() >= k)
            {
                for(T &t : outgoing) this->add_edge_limit_k_and_update(c, t, to_update);
            } else
            {
                std::unordered_set<T> exclude = {c, t1, t2};
                std::vector<T> top_k_elements;
                this->top_k(k, c, exclude, top_k_elements);
                for(T &t : top_k_elements)
                    this->add_edge_limit_k_and_update(c, t, to_update);
                /*
                for(T &t : this->get_all_elements())
                    if(exclude.count(t) == 0)
                        this->add_edge_limit_k_and_update(c, t, to_update);*/
            }
        }

        void update_incoming_edges(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::unordered_set<T> excluded_values = {c, t1, t2};
            replace_incoming_edges(t1, c, excluded_values, to_update);
            replace_incoming_edges(t2, c, excluded_values, to_update);
        }

        void replace_incoming_edges(T &t, T &c, std::unordered_set<T> &excluded_values, std::vector<std::pair<T, T>> &to_update)
        {
            std::vector<T> inc_vec(incoming[t].begin(), incoming[t].end());
            for(T &tn : inc_vec)
            {
                std::vector<T> children;
                this->get_children(children, tn);
                if(children.size() == 0) continue;
                T &last = children.back();
                this->remove_edge_directed(tn, t);
                //if(excluded_values.count(n->get_value()) != 0) continue;

                if(cmp(tn, c) <= cmp(tn, last) && std::find(children.begin(), children.end(), c) == children.end())
                {
                    this->add_edge_limit_k_and_update(tn, c, to_update);
                } else
                {
                    std::unordered_set<T> exclude(excluded_values.begin(), excluded_values.end());
                    for(T &tcc : children) exclude.insert(tcc);
                    exclude.insert(tn);
                    /*
                    for(T &tnn : this->get_all_elements())
                        if(exclude.find(tnn) == exclude.end())
                            add_edge_limit_k_and_update(tn, tnn, to_update);
                    */
                    std::vector<T> top1;
                    top_k(1, tn, exclude, top1);
                    if(top1.size() != 0) this->add_edge_limit_k_and_update(tn, top1[0], to_update);
                }
            }
        }

        void top_k(int k, T &t, std::unordered_set<T> exclude, std::vector<T> &top_elements)
        {
            auto cmp_captured = cmp;
            auto compare = [&t, cmp_captured](T &t1, T &t2) -> float
            {
                return cmp_captured(t, t1) - cmp_captured(t, t2);
            };
            Sorted_Vector<T> top_kek(compare);

            for(T &tn : this->get_all_elements())
            {
                if(exclude.count(tn) != 0) continue;
                if(top_kek.size() < k)
                {
                    top_kek.push(tn);
                } else
                {
                    T &last = top_kek.back();
                    if(cmp(t, tn) < cmp(t, last))
                    {
                        top_kek.erase_back();
                        top_kek.push(tn);
                    }
                }
            }

            top_elements.reserve(k);
            for(T &tn : top_kek)
            {
                top_elements.push_back(tn);
            }
            //std::cout << "k: " << k << ", actual: " << top_elements.size() << std::endl;
        }

    protected:
        Node<T>* create_node(T &t)
        {
            return new Sorted_Node<T>(t, cmp);
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
        KNN_Graph(int k, std::function<float(T&, T&)> cmp): k(k), cmp(cmp) {}

        // 1. create new joined node
        // 2. iterate through all outgoing neighbours of t1, t2
        //    and add if c is <= b_limit
        // 3. if size is less than k, iterate through all nodes
        //    to find the rest
        // 4. iterate through all incoming neighbours of t1, t2,
        //    both will be deleted so replace them
        //    if possible add c, if not then iterate through all
        //    to find the one to replace it with

        void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
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
            update_outgoing_edges(c, t1, t2, to_update);

            //std::cout << "joined children: " << nc->get_children().size() << std::endl;
            // incoming
            update_incoming_edges(c, t1, t2, to_update);
            
            
            
            this->remove_edge(t1, t2);
            this->remove_node(t1);
            this->remove_node(t2);
            
            //std::cout << "match: " << this->match_in_out() << std::endl;
            //std::cout << "gone:\nt1 = " << gone(t1) << "\nt2 = " << gone(t2) << std::endl;
        }

        // override base function to update incoming map
        void remove_node(T &t)
        {
            incoming.erase(t);
            Graph<T>::remove_node(t);
        }

        // override base function to update incoming map
        void add_edge_directed(T &t1, T &t2)
        {
            incoming[t2].insert(t1);
            Graph<T>::add_edge_directed(t1, t2);
        }

        // override base function to update incoming map
        void remove_edge_directed(T &t1, T &t2)
        {
            if(incoming.find(t2) != incoming.end()) incoming[t2].erase(t1);
            Graph<T>::remove_edge_directed(t1, t2);
        }

        void get_neighbours(std::vector<T> &vec, T &t)
        {
            this->get_children(vec, t);
            
        }

        bool match_in_out()
        {
            std::unordered_map<T, std::unordered_set<T>> check;
            std::vector<T> children;

            for(T &t : this->get_all_elements())
            {
                children.clear();
                this->get_children(children, t);
                for(T &tc : children)
                {
                    check[tc].insert(t);
                }
            }

            for(T &t : this->get_all_elements())
            {
                std::unordered_set<T> &children_check = check[t], &children_inc = incoming[t];
                if(children_check.size() != children_inc.size())
                {
                    std::cerr << "size doesn't match (check, inc) = (" + std::to_string(children_check.size()) + ", " + std::to_string(children_inc.size()) + "): ";
                    std::cerr << t->to_string() << "\nchildren_check: ";
                    for(T tc : children_check) std::cerr << tc->to_string() << ", ";
                    std::cerr << "\nchildren_inc: ";
                    for(T tc : children_inc) std::cerr << tc->to_string() << ", ";
                    std::cerr << std::endl;
                    throw std::invalid_argument("");
                    return false;
                }

                Maptor<T> diff;
                for(T t : children_check) diff.push_back(t);
                for(T t : children_inc) diff.erase(t);
                
                if(diff.size() != 0)
                {
                    std::cerr << "elements don't match: ";
                    std::cerr << t->to_string() << "\nchildren_check: ";
                    for(T t : children_check) std::cerr << t->to_string() << ", ";
                    std::cerr << "\nchildren_inc";
                    for(T t : children_inc) std::cerr << t->to_string() << ", ";
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
            std::vector<T> children;

            for(T &tn : this->get_all_elements())
            {
                if(tn == t)
                {
                    throw std::invalid_argument(t->to_string() + " still exists in outgoing as key");
                    return false;
                }
                
                this->get_children(children, tn);
                for(T &tnn : children)
                {
                    if(tnn == t)
                    {
                        throw std::invalid_argument(t->to_string() + " still exists in outgoing as child");
                        return false;
                    }
                }
            }

            // check incoming edges
            for(auto &e : incoming)
            {
                T tn = e.first;
                if(tn == t)
                {
                    throw std::invalid_argument(t->to_string() + " still exists in incoming as key");
                    return false;
                }

                for(T tnn : e.second)
                {
                    if(tnn == t)
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