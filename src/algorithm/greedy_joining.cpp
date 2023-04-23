#include <cfloat>
#include <iostream>

#include "algorithm/greedy_joining.h"
#include "util/util.h"


std::unordered_map<Data*, std::string> Greedy_Joining::execute(std::vector<Data*> input, float dist)
{
    reset_state();
    float cost_total = 0;

    for(Data *d : input)
    {
        Cluster *cl = cls.add_empty_cluster();
        cl->push_back(d);
        for(float f : *d) cl->get_sum().push_back(f);
        cl->sum_of_squares = Util::sum_of_squares(*d);
    }

    for(Cluster *cl : cls)
    {
        f[cl] = 0;
    }

    while(1)
    {
        int num_cls = cls.size();
        float f_best = 1e10, d_best = 1e10;
        int best_pair[] = {-1, -1};

        for(int i = 0; i < num_cls-1; i++)
        {
            Cluster *cl1 = cls[i];
            float cl1_size = cl1->size();
            //std::cout << i << std::endl;
            for(int j = i+1; j < num_cls; j++)
            {
                //std::cout << "(" << i << ", " << j << ")" << std::endl;
                Cluster *cl2 = cls[j];
                float cl2_size = cl2->size();
                float f_diff = cl1_size*cl2->sum_of_squares + cl2_size*cl1->sum_of_squares - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum());
                float d_diff = d(cl1_size, dist) + d(cl2_size, dist) - d(cl1_size+cl2_size, dist);
                
                if(f_diff+d_diff < f_best+d_best)
                {
                    f_best = f_diff;
                    d_best = d_diff;
                    best_pair[0] = i;
                    best_pair[1] = j;
                }
            }
        }

        std::cout << "best pair: (" << best_pair[0] << ", " << best_pair[1] << ")" << std::endl;
        std::cout << "f_best: " << f_best << ", d_best: " << d_best << std::endl;

        // if the score is worse then stop
        if(f_best+d_best >= 0) break;
        
        // store old values for updated value
        Cluster *cl1 = cls[best_pair[0]], *cl2 = cls[best_pair[1]];
        float f_cl1 = f[cl1], f_cl2 = f[cl2];
        f.erase(cl1);
        f.erase(cl2);
        
        // join clusters and update the score
        int idx_joined = cls.join(best_pair[0], best_pair[1]);
        if(idx_joined == -1) continue;
        Cluster *cl_joined = cls[idx_joined];
        f[cl_joined] = f_cl1 + f_cl2 + f_best;

        cost_total += f_best + d_best;
    }
    
    std::unordered_map<Data*, std::string> cluster_map;
    for(int i = 0; i < cls.size(); i++)
    {
        for(auto &it : *cls[i])
        {
            cluster_map[it] = std::to_string(i);
        }
    }

    return cluster_map;
}