#include "eval/evaluation.h"

#ifndef __davies_bouldin_index_include__
#define __davies_bouldin_index_include__

class Davies_Bouldin_Index: public Evaluation
{
    public:
        double execute(std::unordered_map<Data*, std::string> map);
}

#endif