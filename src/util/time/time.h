#include <chrono>

#ifndef __time_include__
#define __time_include__

class Time
{
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    public:
        void start()
        {
            start_time = std::chrono::high_resolution_clock::now();
        }

        double stop()
        {
            return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count();
        }
};

#endif