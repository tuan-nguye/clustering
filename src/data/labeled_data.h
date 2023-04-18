#include "data/data.h"
#include <string>

#ifndef __labeled_data_include__
#define __labeled_data_include__

class Labeled_Data: public Data
{
    public:
        std::string label;
        Labeled_Data() {};
        Labeled_Data(std::string label): label(label) {};
        std::string to_string()
        {
            return "label: " + label + ", " + Data::to_string();
        }
};

#endif