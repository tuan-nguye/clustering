#include <vector>
#include <iostream>
#include <string>

#include "eval/evaluation.h"
#include "partition-comparison.h"

#ifndef __variation_of_information_include__
#define __variation_of_information_include__

class Variation_Of_Information: public Evaluation
{
    public:
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

            andres::VariationOfInformation<double> variation(truth.begin(), truth.end(), prediction.begin());
            return variation.value();
        }
};

#endif