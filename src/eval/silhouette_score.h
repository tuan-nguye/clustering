#include "eval/evaluation.h"

#ifndef __silhouette_score_include__
#define __silhouette_score_include__

class Silhouette_Score: public Evaluation
{
    public:
        double execute(std::unordered_map<Data*, std::string> map);
}

#endif