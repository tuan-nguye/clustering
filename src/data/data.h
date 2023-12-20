#include <vector>
#include <string>

#ifndef __data_include__
#define __data_include__


/**
 * @brief data object which consists of float vector and an optional string label.
 * 
 */
class Data: public std::vector<float>
{
    public:
        // empty contructor
        Data() {}
        /**
         * @brief Construct a new Data object
         * 
         * @param label class label
         */
        Data(std::string label): label(label) {}
        // class label
        std::string label;

        /**
         * @brief string representation of the data object
         * 
         * @return std::string 
         */
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