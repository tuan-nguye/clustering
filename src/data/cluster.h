#include <vector>

#include "data/data.h"
#include "util/util.h"

#ifndef __cluster_include__
#define __cluster_include__


class Cluster: public std::vector<Data*>
{
    private:
        std::vector<float> sum;
        float sum_of_squares = 0;
        float dist;
        float f = 0;
        float score = 0;
    public:
        Cluster(float d): dist(d) {}

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
            // update values
            f = f + cl.get_f() + this->size()*cl.get_sum_of_squares() + cl.size()*this->get_sum_of_squares() - 2*Util::scalar_product(this->sum, cl.get_sum());
            score = f - Util::d_all_pairs(size(), dist);
            add_to_sum(cl.get_sum());
            add_to_sum_of_squares(cl.get_sum_of_squares());

            // add all elements
            insert(end(), cl.begin(), cl.end());

            // delete other cluster object
            delete &cl;
        }

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