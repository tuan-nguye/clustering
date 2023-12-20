#include <vector>

#include "data/data.h"
#include "util/util_math.h"

#ifndef __cluster_include__
#define __cluster_include__


/**
 * @brief Cluster class implementation. Each cluster can contain a number
 * of Data objects. It also stores various variables needed for easily
 * calculating an evaluation function.
 * 
 */
class Cluster: public std::vector<Data*>
{
    private:
        // sum of all the i-th elements of all data elements, resulting in a vector
        std::vector<float> sum;
        // single value indicating the sum of all squared elements of all data elements and all data attributes
        float sum_of_squares = 0;
        // f value, sum of all distance between all pairs of elements
        float f = 0;
        // objective value score of the cluster
        float score = 0;
        // cluster size
        int cl_size = 0;
    public:
        // empty constructor
        Cluster() {}

        /**
         * @brief Construct a new Cluster object
         * 
         * @param d distance threshold
         */
        Cluster(Data *d): sum_of_squares(Util_Math::sum_of_squares(*d)), cl_size(1)
        {
            add_to_sum(*d);
            push_back(d);
        }

        /**
         * @brief Get the sum object
         * 
         * @return std::vector<float>& 
         */
        std::vector<float>& get_sum() { return sum; }

        /**
         * @brief add a vector of floats to the sum vector
         * 
         * @param vec vector to be added
         */
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

        /**
         * @brief add value to the sum_of_squares variable
         * 
         * @param f 
         */
        void add_to_sum_of_squares(float f) { sum_of_squares += f; }

        /**
         * @brief Get the sum of squares object
         * 
         * @return float 
         */
        float get_sum_of_squares() { return sum_of_squares; }

        /**
         * @brief Get the f value of the cluster
         * 
         * @return float 
         */
        float get_f() { return f; }

        /**
         * @brief Set the f value
         * 
         * @param value 
         */
        void set_f(float value) { f = value; }

        /**
         * @brief Get the score of the cluster
         * 
         * @return float 
         */
        float get_score() { return score; }

        /**
         * @brief Set the score
         * 
         * @param value 
         */
        void set_score(float value) { score = value; }

        /**
         * @brief returns the size of the cluster which is the number of data objects
         * it should contain. This variable can be changed at will and increased
         * even if no data object was added.
         * 
         * @return int 
         */
        int size() { return cl_size; }

        /**
         * @brief Set the size
         * 
         * @param new_size 
         */
        void set_size(int new_size) { cl_size = new_size; }

        /**
         * @brief returns the actual number of data objects contained
         * 
         * @return int 
         */
        int data_size() { return std::vector<Data*>::size(); }

        /**
         * @brief get string representation of the cluster
         * 
         * @return std::string 
         */
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