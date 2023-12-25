#include "eval/evaluation.h"

#ifndef __davies_bouldin_index_include__
#define __davies_bouldin_index_include__

/**
 * @brief Davies Building index for clustering evaluation. TODO
 * 
 */
class Davies_Bouldin_Index: public Evaluation
{
    public:
        double execute(std::unordered_map<Data*, std::string> map);
};

#endif