#include "data.h"
#include <string>

#ifndef __labeled_data_include__
#define __labeled_data_include__

class Labeled_Data: Data
{
    public:
        std::string label;
        Labeled_Data(std::string label): label(label) {};
};

#endif