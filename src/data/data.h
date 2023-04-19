#include <vector>
#include <string>

#ifndef __data_include__
#define __data_include__

class Data
{
    public:
        Data() {}
        Data(std::string label): label(label) {}
        std::vector<float> attributes;
        std::string label;

        std::string to_string()
        {
            std::string out = "label: " + label + ", [";
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