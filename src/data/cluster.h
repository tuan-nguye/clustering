#include <vector>

#include "data/data.h"

#ifndef __cluster_include__
#define __cluster_include__


class Cluster: public std::vector<Data*>
{
    private:
        std::vector<float> sum;
    public:
        float sum_of_squares = 0;
        std::vector<float>& get_sum() { return sum; }
        void join(Cluster& cl)
        {
            std::vector<float> &sum_other = cl.get_sum();
            for(int i = 0; i < sum.size(); i++) sum[i] += sum_other[i];
            sum_of_squares += cl.sum_of_squares; 
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