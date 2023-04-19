#include <vector>
#include <iostream>
#include <string>

#ifndef __parser_include__
#define __parser_include__

class Data;

class Parser
{
    public:
        virtual void parse(std::vector<Data*> &data, std::string datafile_path) = 0;
        virtual void parse(std::vector<Data*> &data, std::string datafile_path, std::string labelfile_path) = 0;
};

#endif