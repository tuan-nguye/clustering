#include <vector>
#include <string>

#ifndef __data_include__
#define __data_include__

class Data: public std::vector<float>
{
    public:
        Data() {}
        Data(std::string label): label(label) {}
        std::string label;

        std::string to_string()
        {
            std::string out = "(" + label + ": ";
            bool first = true;

            for(float f : *this)
            {
                if(first) first = false;
                else out += ", ";
                out += std::to_string(f);
            }

            out += ")";
            return out;
        };
};

#endif