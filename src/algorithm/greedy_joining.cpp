#include <cfloat>
#include <iostream>

#include "algorithm/greedy_joining.h"
#include "util/util.h"


std::unordered_map<Data*, std::string> Greedy_Joining::execute(std::vector<Data*> input, float dist)
{
    reset_state();
    Cluster_Graph cls_graph(dist);
    for(Data *d : input) cls_graph.add_data(d);
    cls_graph.init_clusters_fine_grained();

    float cost_total = 0;

    for(Cluster *cl : cls_graph)
    {
        f[cl] = 0;
        //for(float f : cl->get_sum()) std::cout << f << ", ";
        //std::cout << std::endl;
    }

    while(1)
    {
        int num_cls = cls_graph.size();
        float f_best = 1e38, d_best = 1e38;
        Cluster *best_pair[] = {nullptr, nullptr};
        //std::cout << "cls_graph.size() = " << num_cls << std::endl;

        for(Cluster *cl1 : cls_graph)
        {
            float cl1_size = cl1->size();
            //std::cout << i << std::endl;
            //std::cout << cl1->to_string() << ", children: ";
            std::vector<Cluster*> children;
            cls_graph.get_neighbours(children, cl1);
            for(Cluster *cl2 : children)
            {
                //std::cout << cl2->to_string() << ", ";
                //std::cout << "(" << i << ", " << j << ")" << std::endl;
                float cl2_size = cl2->size();
                float f_diff = cl1_size*cl2->get_sum_of_squares() + cl2_size*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum());
                float d_diff = d(cl1_size, dist) + d(cl2_size, dist) - d(cl1_size+cl2_size, dist);
                
                if(f_diff+d_diff < f_best+d_best)
                {
                    f_best = f_diff;
                    d_best = d_diff;
                    best_pair[0] = cl1;
                    best_pair[1] = cl2;
                }
            }
            //std::cout << std::endl;;
        }

        //if(best_pair[0] != nullptr) std::cout << "best pair: (" << best_pair[0]->to_string() << ", " << best_pair[1]->to_string() << ")" << std::endl;
        std::cout << "f_best: " << f_best << ", d_best: " << d_best << std::endl;

        // if the score is worse then stop
        if(f_best+d_best >= 0) break;
        
        // store old values for updated value
        Cluster *cl1 = best_pair[0], *cl2 = best_pair[1];
        float f_cl1 = f[cl1], f_cl2 = f[cl2];
        f.erase(cl1);
        f.erase(cl2);
        
        // join clusters and update the score
        Cluster *cl_joined = cls_graph.join(best_pair[0], best_pair[1]);
        f[cl_joined] = f_cl1 + f_cl2 + f_best;

        cost_total += f_best + d_best;
    }
    
    std::unordered_map<Data*, std::string> cluster_map;
    int label = 0;
    for(auto &cl : cls_graph)
    {
        for(auto &elem : *cl)
        {
            cluster_map[elem] = std::to_string(label);
        }
        label++;
    }

    return cluster_map;
}