#include <vector>

#include "data/data.h"

#ifndef __cluster_include__
#define __cluster_include__


class Cluster: public std::vector<Data*>
{
    private:
        std::vector<float> sum;
        float sum_of_squares = 0;
    public:
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

        void join(Cluster& cl)
        {
            // add all elements
            insert(end(), cl.begin(), cl.end());
            // update values
            std::vector<float> &sum_other = cl.get_sum();
            add_to_sum(cl.get_sum());
            add_to_sum_of_squares(cl.get_sum_of_squares());
        }

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