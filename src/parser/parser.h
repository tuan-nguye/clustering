#include <vector>
#include <iostream>

class Data;

class Parser
{
    public:
        Parser() {};
        virtual std::vector<Data*> parse() = 0;
};