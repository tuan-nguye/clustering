#include <unordered_map>
#include <iostream>
#include <string>

#include "data/cluster.h"
#include "data/graph/cluster_graph.h"

#ifndef __clustering_include__
#define __clustering_include__

class Clustering
{
    private:
        double objective_value = 0.0;
        bool parallel = false;
    protected:
        void set_objective_value(double val) { objective_value = val; }
    public:
        // execute the clustering algorithm provided the input data and the distance d
        virtual std::unordered_map<Data*, std::string> execute(std::vector<Data*> &input, float d) = 0;
        
        // generate a label for each postive number
        std::string generate_label(int n)
        {
            std::string label;

            while(n >= 0)
            {
                label = char('a' + n%26) + label;
                n = n/26 - 1;
            }

            return label;
        }

        double get_objective_value() { return objective_value; }
        bool parallel_enabled() { return parallel; }
        void set_parallel(bool value) { parallel = value; }
};

#endif