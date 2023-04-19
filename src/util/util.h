#include <vector>

#ifndef __util_include__
#define __util_include__

class Util
{
    public:
        static double square_sum(std::vector<float> vals);
        static double magnitude(std::vector<float> vals);
        static double scalar_product(std::vector<float> v1, std::vector<float> v2);
};

#endif