#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "data/graph/sorted_node.h"
#include "data/structures/maptor.h"

#ifndef __knn_graph_include__
#define __knn_graph_include__


template<typename T> class KNN_Graph: public Auto_Edge_Graph<T>
{
    private:
        // comparison function for evaluating distance between two clusters
        std::function<float(T&, T&)> distance;
        // distance constant
        float dist;
        // k constant, number of neighbours
        int k;
        // map storing all incoming neighbours of a value
        std::unordered_map<T, std::unordered_set<T>> incoming;

    protected:
        virtual void update_outgoing_edges(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::vector<T> children1, children2;
            this->get_children(children1, t1);
            this->get_children(children2, t2);
            Maptor<T> outgoing;
            outgoing.reserve(2*k);
            float b_limit = 0.0f;
            if(children1.size() != 0) b_limit += distance(t1, children1.back());
            if(children2.size() != 0) b_limit += distance(t2, children2.back());
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
                if(distance(c, tn1) > b_limit) continue;
                outgoing.push_back(tn1);
            }

            for(T &tn2 : children2)
            {
                incoming[tn2].erase(t2);
                if(distance(c, tn2) > b_limit) continue;
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
                std::vector<T> topk;
                std::unordered_set<T> empty;
                top_k(k, c, outgoing.get_vector(), empty, topk);
                for(T &t : topk) add_edge_directed_update(c, t, to_update);
            } else
            {
                update_outgoing_edges_all(c, t1, t2, to_update);
            }
        }

        virtual void update_outgoing_edges_all(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::unordered_set<T> exclude = {c, t1, t2};
            std::vector<T> top_k_elements;
            this->top_k(k, c, this->get_all_elements().get_vector(), exclude, top_k_elements);
            for(T &t : top_k_elements)
                add_edge_directed_update(c, t, to_update);
        }

        void update_incoming_edges(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            std::unordered_set<T> excluded_values = {c, t1, t2};
            Maptor<T> to_replace;
            to_replace.reserve(incoming[t1].size()+incoming[t2].size());

            for(T tn1 : incoming[t1])
            {
                to_replace.push_back(tn1);
                Graph<T>::remove_edge_directed(tn1, t1);
            }

            for(T tn2 : incoming[t2])
            {
                to_replace.push_back(tn2);
                Graph<T>::remove_edge_directed(tn2, t2);
            }

            to_replace.erase(t1);
            to_replace.erase(t2);
            for(T &tn : to_replace)
            {
                if(this->number_of_children(tn) == 0) continue;

                T &last = this->last_child(tn);
                if(distance(tn, c) < distance(tn, last)) add_edge_directed_update(tn, c, to_update);
                else replace_incoming_edges_all(tn, excluded_values, to_update);
            }
        }

        virtual void replace_incoming_edges_all(T &t, std::unordered_set<T> exclude, std::vector<std::pair<T, T>> &to_update)
        {
            // erase elements that are children already or itself
            std::vector<T> children;
            this->get_children(children, t);
            for(T &t : children) exclude.insert(t);
            exclude.insert(t);
            
            if(this->size() <= exclude.size()) return;
            std::vector<T> top1;
            top_k(1, t, this->get_all_elements().get_vector(), exclude, top1);
            add_edge_directed_update(t, top1[0], to_update);
        }

        Node<T>* create_node(T &t)
        {
            return new Sorted_Node<T>(t, distance);
        }

        virtual void add_edges_operation(T &t, std::mutex *mtx)
        {
            std::vector<T> topk;
            std::unordered_set<T> exclude = {t};
            top_k(k, t, this->get_all_elements().get_vector(), exclude, topk);

            if(mtx == nullptr) 
            {
                for(T &tn : topk) this->add_edge_directed(t, tn);
            } else
            {
                std::lock_guard<std::mutex> lock(*mtx);
                for(T &tn : topk) this->add_edge_directed(t, tn);
            }
        }

        // returns cmp function for sorted data structures
        // compares distance of two values to &t
        std::function<float(T&, T&)> cmp_function(T &t)
        {
            //std::function<float(T&, T&)> cmp_captured = distance;
            std::function<float(T&, T&)> eval_nearest_neighbour = [&t, this](T &t1, T &t2) -> float
            {
                return this->distance(t, t1) - this->distance(t, t2);
            };
            return eval_nearest_neighbour;
        }

        // returns cmp function for sorted data structures
        // returns bool whether first element is smaller than second
        std::function<bool(T, T)> cmp_function_bool(T &t)
        {
            std::function<bool(T, T)> eval_compare = [&t, this](T t1, T t2) -> float
            {
                return this->distance(t, t1) < this->distance(t, t2);
            };

            return eval_compare;
        }

        // finds the top k elements according to the comparison function
        // and stores them in top_elements
        void top_k(int k, T &t, std::vector<T> &candidates, std::unordered_set<T> &exclude, std::vector<T> &top_elements)
        {
            Sorted_Vector<T> top_kek(cmp_function(t));

            for(T &tn : candidates)
            {
                if(exclude.count(tn) != 0) continue;
                if(top_kek.size() < k)
                {
                    top_kek.push(tn);
                } else
                {
                    T &last = top_kek.back();
                    if(distance(t, tn) < distance(t, last))
                    {
                        top_kek.pop_back();
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

        int get_k() { return k; }
        
        int get_d() { return dist; }

        std::function<float(T&, T&)>& get_distance_function() { return distance; }
    public:
        KNN_Graph(int k, std::function<float(T&, T&)> distance): k(k), distance(distance) {}

        // 1. create new joined node
        // 2. iterate through all outgoing neighbours of t1, t2
        //    and add if c is <= b_limit
        // 3. if size is less than k, iterate through all nodes
        //    to find the rest
        // 4. iterate through all incoming neighbours of t1, t2,
        //    both will be deleted so replace them
        //    if possible add c, if not then iterate through all
        //    to find the one to replace it with

        virtual void combine_nodes_into(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
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

            this->add_node(c);
            update_outgoing_edges(c, t1, t2, to_update);

            //std::cout << "joined children: " << nc->get_children().size() << std::endl;
            // incoming
            update_incoming_edges(c, t1, t2, to_update);
            
            
            
            this->remove_edge(t1, t2);
            this->remove_node(t1);
            this->remove_node(t2);
            
            //std::cout << "match: " << this->match_in_out() << std::endl;
            //std::cout << "gone:\nt1 = " << gone(t1) << "\nt2 = " << gone(t2) << std::endl;
            //this->print_structure();
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

        void add_edge_directed_update(T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            add_edge_directed(t1, t2);
            to_update.emplace_back(t1, t2);
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

        void clear()
        {
            incoming.clear();
            Auto_Edge_Graph<T>::clear();
        }

        void clear_edges()
        {
            incoming.clear();
            Auto_Edge_Graph<T>::clear_edges();
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