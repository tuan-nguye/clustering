#include <unordered_map>
#include <string>

#include "data/data.h"

#ifndef __evaluation_include__
#define __evaluation_include__

class Evaluation
{
    protected:
        std::vector<std::pair<Data*, std::string>> get_entry_vector(std::unordered_map<Data*, std::string> map)
        {
            std::vector<std::pair<Data*, std::string>> entries;
            for(auto &entry : map) entries.push_back(entry);
            return entries;
        }
    public:
        virtual double execute(std::unordered_map<Data*, std::string> prediction_map) = 0;
};

#endif