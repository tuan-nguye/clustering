#include "data.h"
#include <string>

class Labeled_Data: Data
{
    public:
        std::string label;
        Labeled_Data(std::string label): label(label) {};
};