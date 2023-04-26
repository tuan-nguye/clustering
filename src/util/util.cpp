#include <cmath>
#include <stdexcept>
#include <numeric>

#include "util.h"

float Util::sum_of_squares(std::vector<float> &vals)
{
    float square_magnitude = 0.0;

    for(float f : vals)
    {
        square_magnitude += f*f;
    }

    return square_magnitude;
}

float Util::magnitude(std::vector<float> &vals)
{
    return (float) sqrt(sum_of_squares(vals));
}

float Util::scalar_product(std::vector<float> &v1, std::vector<float> &v2)
{
    if(v1.size() != v2.size()) throw std::invalid_argument("vectors have different size, can't calculate scalar product");
    
    float scalar = 0.0f;

    for(int i = 0; i < v1.size(); i++)
    {
        scalar += v1[i]*v2[i];
    }

    return scalar;
}

float Util::inner_product(std::vector<float> &v1, std::vector<float> &v2)
{
    if(v1.size() != v2.size()) throw std::invalid_argument("vectors have different size, can't calculate scalar product");
    float scalar = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0f);
    return scalar;
}

float Util::euclidean_distance(std::vector<float> &v1, std::vector<float> &v2)
{
    if(v1.size() != v2.size()) throw std::invalid_argument("vectors have different size, can't calculate scalar product");
    
    float dist = 0.0f;

    for(int i = 0; i < v1.size(); i++)
    {
        float diff = v1[i]-v2[i];
        dist += diff*diff;
    }

    return sqrt(dist);
}

float Util::d_all_pairs(int size, float d)
{
    return (size*(size-1))/2 * d*d;
}