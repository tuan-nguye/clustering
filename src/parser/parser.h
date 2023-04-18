#include <vector>
#include <iostream>
#include <string>

#ifndef __parser_include__
#define __parser_include__

class Data;

class Parser
{
    public:
        virtual void parse(std::vector<Data*> &data, std::string file_path) = 0;
};

#endif