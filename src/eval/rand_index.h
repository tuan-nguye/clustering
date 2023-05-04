#include <vector>
#include <iostream>

#include "eval/evaluation.h"

#ifndef __rand_index_include__
#define __rand_index_include__

/*
iterate through all pairs of elements and count the number
of pairs that are in the same subset in both labelings
and pairs that are in different subsets in both labelings
*/
class Rand_Index: public Evaluation
{
    public:
        double execute(std::unordered_map<Data*, std::string> map)
        {
            std::vector<std::pair<Data*, std::string>> entries = get_entry_vector(map);
            long long n = entries.size();
            long long correct = 0;

            for(int i = 0; i < n-1; i++)
            {
                for(int j = i+1; j < n; j++)
                {
                    bool same1 = entries[i].first->label == entries[j].first->label;
                    bool same2 = entries[i].second == entries[j].second;

                    if(same1 == same2) correct++;
                }
            }

            return double(correct) / ((n*(n-1))/2l);
        }
};

#endif