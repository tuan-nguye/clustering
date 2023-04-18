#include <vector>

#ifndef __data_include__
#define __data_include__

class Data
{
    public:
        std::vector<float> attributes;
        
        std::string to_string()
        {
            std::string out = "[";
            bool first = true;

            for(float f : attributes)
            {
                if(first) first = false;
                else out += ", ";
                out += std::to_string(f);
            }

            out += "]";
            return out;
        };
};

#endif