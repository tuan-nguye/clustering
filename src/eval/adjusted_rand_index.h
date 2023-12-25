#include <unordered_set>

#include "eval/evaluation.h"

#ifndef __adjusted_rand_index_include__
#define __adjusted_rand_index_include__

/**
 * @brief Implementation of the Adjusted Rand index for clustering evaluation, range: [-1, 1], where 1 is the best value.
 * from: https://en.wikipedia.org/wiki/Rand_index#:~:text=The%20adjusted%20Rand%20index%20is,specified%20by%20a%20random%20model.
 * 
 */
class Adjusted_Rand_Index: public Evaluation
{
    public:
        double execute(std::unordered_map<Data*, std::string> map)
        {
            std::vector<std::pair<Data*, std::string>> entries = get_entry_vector(map);
            int n = entries.size();

            std::unordered_set<std::string> labels_orig, labels_clus;

            for(auto &entry : entries)
            {
                labels_orig.insert(entry.first->label);
                labels_clus.insert(entry.second);
            }

            int num_label_orig = labels_orig.size(), num_label_clus = labels_clus.size();

            std::unordered_map<std::string, std::unordered_map<std::string, int>> contingency_table;
            std::unordered_map<std::string, long> count_orig, count_clus;
            
            for(auto &entry : entries)
            {
                std::string label_orig = entry.first->label;
                std::string label_clus = entry.second;

                if(count_orig.find(label_orig) == count_orig.end()) count_orig[label_orig] = 1;
                else count_orig[label_orig]++;

                if(count_clus.find(label_clus) == count_clus.end()) count_clus[label_clus] = 1;
                else count_clus[label_clus]++;

                if(contingency_table.find(label_orig) == contingency_table.end())
                    contingency_table[label_orig] = std::unordered_map<std::string, int>();
                
                std::unordered_map<std::string, int> &count = contingency_table[label_orig];

                if(count.find(label_clus) == count.end()) count[label_clus] = 0;

                count[label_clus]++;
            }

            long sum_orig_2 = 0;
            for(auto &e : count_orig)
            {
                long sum = e.second;
                sum_orig_2 += sum*(sum-1)/2;
            }
            
            long sum_clus_2 = 0;
            for(auto &e : count_clus)
            {
                long sum = e.second;
                sum_clus_2 += sum*(sum-1)/2;
            }

            long n_2 = n*(n-1)/2;

            long sum_entries_2 = 0;

            for(auto &e1 : contingency_table)
            {
                for(auto &e2 : e1.second)
                {
                    long sum = e2.second;
                    sum_entries_2 += sum*(sum-1)/2;
                }
            }

            return double(sum_entries_2-(sum_orig_2*sum_clus_2)/n_2)/(0.5*(sum_orig_2+sum_clus_2) - (sum_orig_2*sum_clus_2)/n_2);
        }
};

#endif