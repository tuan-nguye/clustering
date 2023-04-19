#include <cmath>
#include <stdexcept>
#include <numeric>

#include "util.h"

double Util::square_sum(std::vector<float> vals)
{
    double square_magnitude = 0.0;

    for(float f : vals)
    {
        square_magnitude += f*f;
    }

    return square_magnitude;
}

double Util::magnitude(std::vector<float> vals)
{
    return sqrt(square_sum(vals));
}

double Util::scalar_product(std::vector<float> v1, std::vector<float> v2)
{
    if(v1.size() != v2.size()) throw std::invalid_argument("vectors have different size, can't calculate scalar product");
    double scalar = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0);
    return scalar;
}