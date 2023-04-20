#include <vector>

#ifndef __util_include__
#define __util_include__

class Util
{
    public:
        static float sum_of_squares(std::vector<float> vals);
        static float magnitude(std::vector<float> vals);
        static float scalar_product(std::vector<float> v1, std::vector<float> v2);
        static float inner_product(std::vector<float> v1, std::vector<float> v2);
};

#endif