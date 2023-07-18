#include <vector>
#include <atomic>

#ifndef __util_math_include__
#define __util_math_include__

class Util_Math
{
    public:
        static std::atomic<size_t> call_count;

        static float sum_of_squares(std::vector<float> &vals);
        static float magnitude(std::vector<float> &vals);
        static float scalar_product(std::vector<float> &v1, std::vector<float> &v2);
        static float inner_product(std::vector<float> &v1, std::vector<float> &v2);
        static float euclidean_distance(std::vector<float> &v1, std::vector<float> &v2);
        static float d_all_pairs(int size, float d);
};

#endif