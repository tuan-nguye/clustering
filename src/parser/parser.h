#include <vector>
#include <iostream>
#include "data/data.h"

#ifndef __parser_include__
#define __parser_include__

class Parser
{
    public:
        Parser() {};
        virtual std::vector<Data> parse() = 0;
};

#endif