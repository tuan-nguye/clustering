#include "eval/evaluation.h"

#ifndef __mutual_information_include__
#define __mutual_information_include__

/**
 * @brief Mutual information clustering evaluation. TODO
 * 
 */
class Mutual_Information: public Evaluation
{
    public:
        double execute(std::unordered_map<Data*, std::string> map);
};

#endif