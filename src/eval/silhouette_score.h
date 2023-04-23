#include "eval/evaluation.h"
#include "util/util.h"

#ifndef __silhouette_score_include__
#define __silhouette_score_include__

class Silhouette_Score: public Evaluation
{
    public:
        double execute(std::unordered_map<Data*, std::string> map)
        {
            std::unordered_map<std::string, std::vector<Data*>> clusters;
            for(auto &entry : map)
            {
                if(clusters.find(entry.second) == clusters.end()) clusters[entry.second] = std::vector<Data*>();
                clusters[entry.second].push_back(entry.first);
            }

            std::unordered_map<Data*, double> a, b;

            for(auto &e1 : clusters)
            {
                // calc a
                std::vector<Data*> &cl = e1.second;

                if(cl.size() == 1)
                {
                    a[cl[0]] = 0;
                } else
                {
                    for(auto &d1 : cl)
                    {
                        float dist_sum = 0.0f;
                        for(auto &d2 : cl)
                        {
                            if(d1 == d2) continue;
                            dist_sum += Util::euclidean_distance(*d1, *d2);
                        }
                        a[d1] = dist_sum / (cl.size()-1);
                    }
                }

                // calc b
                for(auto &e2 : clusters)
                {
                    if(e1 == e2) continue;

                    
                }
            }

            return 0.0;
        }
};

#endif