
#include "data/graph/knn_graph.h"
#include "hnswlib/hnswalg.h"
#include "hnswlib/hnswlib.h"
#include "util/util_cluster.h"
#include "data/cluster.h"

#ifndef __lazy_ann_graph_include__
#define __lazy_ann_graph_include__

/**
 * @brief Wrapper class to adapt the evaluation function to the hnswlib
 * 
 * @tparam T 
 */
template<typename T> class Space_Wrapper: public hnswlib::SpaceInterface<float>
{
    private:
        hnswlib::DISTFUNC<float> dist_func;
        std::function<float(T&, T&)> p_eval;
        size_t dim;
        size_t data_size;
    public:
        Space_Wrapper(std::function<float(T&, T&)> eval): p_eval(eval), dim(1), data_size(dim*sizeof(T))
        {
            dist_func = [](const void *vp_t1, const void *vp_t2, const void *p_eval)
            {
                T *t1 = (T*) vp_t1;
                T *t2 = (T*) vp_t2;
                auto eval = *((std::function<float(T&, T&)>*) p_eval);
                return eval(*t1, *t2);
            };
        }

        size_t get_data_size()
        {
            return data_size;
        }

        hnswlib::DISTFUNC<float> get_dist_func()
        {
            return dist_func;
        }

        void *get_dist_func_param()
        {
            return &p_eval;
        }
};

extern int num_threads;

/**
 * @brief Lazy approximated implementation of a KNN-Graph. The condition that every node has exactly
 * k neighbours is not always fulfilled. Nodes can have less edges now because it's being updated
 * less frequently. It's a tradeoff between exact representation and time complexity.
 * 
 * @tparam T 
 */
template<typename T> class Lazy_ANN_Graph: public KNN_Graph<T>
{
    private:
        // constants needed for hnswlib graph implementation
        int M;
        int ef_construction;
        Space_Wrapper<T> space;
        // navigable small world graph
        hnswlib::HierarchicalNSW<float>* hnsw;

        // label_map for constructing hnsw
        std::unordered_map<T, hnswlib::labeltype> label_map;

        /**
         * @brief search for k nearest neighbours for each node and add its edges.
         * can run in parallel or single
         * 
         */
        void knn_search()
        {
            if(this->get_parallel()) knn_search_parallel();
            else knn_search_single();
        }

        /**
         * @brief parallel implementation of knn_search
         * 
         */
        void knn_search_parallel()
        {
            int size = this->size();
            std::mutex mtx;
            std::vector<std::thread> threads;

            for(int i = 0; i < num_threads; i++)
            {
                int start = i*size/num_threads;
                int end = (i+1)*size/num_threads;
                if(start == end) continue;
                threads.emplace_back(&Lazy_ANN_Graph::knn_search_parallel_thread, this, &mtx, start, end);
            }

            for(std::thread &thread : threads)
            {
                thread.join();
            }
        }

        /**
         * @brief function called for each thread for knn-search in parallel
         * 
         * @param mtx 
         * @param start 
         * @param end 
         */
        void knn_search_parallel_thread(std::mutex *mtx, int start, int end)
        {
            Maptor<T> &elements = this->get_all_elements();

            for(int i = start; i < end; i++)
            {
                knn_search_operation(elements[i], mtx);
            }
        }

        /**
         * @brief single threaded implementation for searching for each node's k-
         * nearest neighbours
         * 
         */
        void knn_search_single()
        {
            for(T &t : this->get_all_elements()) knn_search_operation(t);
        }

        // search for knn in hnsw
        // k+1 because it can contain itself
        void knn_search_operation(T &t, std::mutex *mtx)
        {
            int k = this->get_k();
            Maptor<T> &elem = this->get_all_elements();
            std::priority_queue<std::pair<float, hnswlib::labeltype>> knn = hnsw->searchKnn((void*) &t, k+1);
            int count = 0;
            while(!knn.empty() && count < k)
            {
                T &tk = elem[knn.top().second];
                knn.pop();  // pop the first element because it's always itself
                if(t == tk) continue;
                count++;
                std::lock_guard<std::mutex> lock(*mtx);
                this->add_edge_directed(t, tk);
            }
        }
    protected:
        // add a new node to the graph
        void add_edges_operation(T &t, std::mutex *mtx)
        {
            hnsw->addPoint((void*) &t, label_map[t]);
        }

        /**
         * @brief build the entire graph by searching for each node's k nearest
         * neighbours and adding new edges
         * 
         */
        void build_graph()
        {
            // build new hnsw
            size_t max_elements = this->size();
            Maptor<T> &elements = this->get_all_elements();
            for(size_t i = 0; i < max_elements; i++) label_map[elements[i]] = i;
            hnsw = new hnswlib::HierarchicalNSW<float>(&space, max_elements, M, ef_construction);
            
            // add the knn edges to the graph
            KNN_Graph<T>::build_graph();
            knn_search_parallel();
            delete hnsw;

            std::cout << "hnsw built, number of edges = " << this->size_edges() << std::endl;
        }

        /**
         * @brief Empty function because the lazy implementation avoids iterating through
         * all nodes for performance reasons.
         * 
         * @param c 
         * @param t1 
         * @param t2 
         * @param to_update 
         */
        void update_outgoing_edges_all(T &c, T &t1, T &t2, std::vector<std::pair<T, T>> &to_update)
        {
            // leave empty, lazy implementation
        }

        /**
         * @brief Empty function because the lazy implementation avoids iterating through
         * all nodes for performance reasons.
         * 
         * @param t 
         * @param exclude 
         * @param to_update 
         */
        void replace_incoming_edges_all(T &t, std::unordered_set<T> exclude, std::vector<std::pair<T, T>> &to_update)
        {
            // leave empty, lazy implementation
        }
    public:
        /**
         * @brief Construct a new Lazy_ANN_Graph object
         * 
         * @param k number of children for each node, not guaranteed at all times because it's a lazy implementation.
         * The actual number of children can be less.
         * @param M parameter needed for the hnswlib implementation
         * @param ef_construction parameter needed for the hnswlib implementation
         * @param cmp comparison function
         */
        Lazy_ANN_Graph(int k, int M, int ef_construction, std::function<float(T&, T&)> cmp):
        KNN_Graph<T>(k, cmp),
        M(M),
        ef_construction(ef_construction),
        space(cmp)
        {

        }

        /**
         * @brief rebuild the entire graph by finding each node's k nearest neighbours.
         * 
         * @param to_update 
         */
        void rebuild(std::vector<std::pair<T, T>> &to_update)
        {
            // clear current state
            this->clear_edges();
            label_map.clear();

            // rebuild
            this->build_graph();

            // add pairs to update vector
            for(T &t : this->get_all_elements())
            {
                std::vector<T> children;
                this->get_neighbours(children, t);
                for(T &tc : children)
                {
                    to_update.emplace_back(t, tc);
                }
            }
        }
};

#endif