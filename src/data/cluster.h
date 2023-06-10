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

        static Cluster& join(Cluster *cl1, Cluster *cl2)
        {
            Cluster* cl_joined = new Cluster(cl1->get_distance());
            cl_joined->set_f(cl1->get_f() + cl2->get_f() + cl1->size()*cl2->get_sum_of_squares() + cl2->size()*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum()));
            cl_joined->set_score(cl_joined->get_f()-Util::d_all_pairs(cl1->size()+cl2->size(), cl_joined->get_distance()));

            cl_joined->add_to_sum(cl1->get_sum());
            cl_joined->add_to_sum(cl2->get_sum());

            cl_joined->add_to_sum_of_squares(cl1->get_sum_of_squares());
            cl_joined->add_to_sum_of_squares(cl2->get_sum_of_squares());

            cl_joined->insert(cl_joined->end(), cl1->begin(), cl1->end());
            cl_joined->insert(cl_joined->end(), cl2->begin(), cl2->end());

            return *cl_joined;
        }

        float get_f() { return f; }

        void set_f(float value) { f = value; }

        float get_score() { return score; }

        void set_score(float value) { score = value; }

        float get_distance() { return dist; }

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