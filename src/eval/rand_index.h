#include <vector>
#include <iostream>
#include <string>

#include "eval/evaluation.h"
#include "partition-comparison.h"

#ifndef __rand_index_include__
#define __rand_index_include__

/**
 * @brief Rand index implementation. iterate through all pairs of elements and count the number
 * of pairs that are in the same subset in both labelings
 * and pairs that are in different subsets in both labelings
 * The range goes from [0, 1] where 1 is the best possible value.
 */
class Rand_Index: public Evaluation
{
    private:
        /**
         * @brief my own rand index implementation.
         * 
         * @param prediction_map 
         * @return double 
         */
        double deprecated_implementation(std::unordered_map<Data*, std::string> prediction_map)
        {
            std::vector<std::pair<Data*, std::string>> entries = get_entry_vector(prediction_map);
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

        double recall_joins = 0.0;
        double recall_cuts = 0.0;
    public:
        /**
         * @brief Rand index implementation from the andres library
         * 
         * @param prediction_map 
         * @return double 
         */
        double execute(std::unordered_map<Data*, std::string> prediction_map)
        {
            // build map to assign each truth/prediction label unique size_t value
            // adaptation to use Rand_Error class
            std::unordered_map<std::string, size_t> truth_to_sizet, pred_to_sizet;
            size_t idx_truth = 0, idx_pred = 0;

            for(auto &entry : prediction_map)
            {
                Data *d = entry.first;
                std::string truth_label = d->label, pred_label = entry.second;
                
                if(truth_to_sizet.count(truth_label) == 0)
                {
                    truth_to_sizet[truth_label] = idx_truth++;
                }

                if(pred_to_sizet.count(pred_label) == 0)
                {
                    pred_to_sizet[pred_label] = idx_pred++;
                }
            }

            // build iterators; parameters for Rand_Error

            int map_size = prediction_map.size();
            std::vector<size_t> truth(map_size), prediction(map_size);

            int idx = 0;
            for(auto &entry : prediction_map)
            {
                Data *d = entry.first;
                std::string truth_label = d->label, pred_label = entry.second;
                truth[idx] = truth_to_sizet[truth_label];
                prediction[idx] = pred_to_sizet[pred_label];
                idx++;
            }

            andres::RandError<double> rand_index(truth.begin(), truth.end(), prediction.begin());
            recall_joins = rand_index.recallOfJoins();
            recall_cuts = rand_index.recallOfCuts();
            return rand_index.index();
        }

        // get the recall score of the joins (elements in the same cluster)
        double get_recall_joins() { return recall_joins; }
        // get the recall score of the cuts (elements in different clusters)
        double get_recall_cuts() { return recall_cuts; }
};

#endif