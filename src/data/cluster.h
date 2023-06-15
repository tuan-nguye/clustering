#include <vector>

#include "data/data.h"

#ifndef __cluster_include__
#define __cluster_include__

class Cluster: public std::vector<Data*>
{
    private:
        std::vector<float> sum;
        float sum_of_squares = 0;
        float f = 0;
        float score = 0;
    public:
        Cluster() {}

        std::vector<float>& get_sum() { return sum; }

        void add_to_sum(std::vector<float>& vec)
        {
            if(sum.size() < vec.size()) sum.reserve(vec.size());
            if(sum.empty())
            {
                for(int i = 0; i < vec.size(); i++)
                {
                    sum.push_back(vec[i]);
                }
            } else
            {
                for(int i = 0; i < sum.size(); i++)
                {
                    sum[i] += vec[i];
                }
            }
            
        }

        void add_to_sum_of_squares(float f) { sum_of_squares += f; }

        float get_sum_of_squares() { return sum_of_squares; }

        float get_f() { return f; }

        void set_f(float value) { f = value; }

        float get_score() { return score; }

        void set_score(float value) { score = value; }

        std::string to_string()
        {
            std::string out = "Cluster[";
            bool first = true;

            for(Data *data : *this)
            {
                if(first) first = false;
                else out += ", ";
                out += data->to_string();
            }

            out += "]";
            return out;
        }
};

#endif