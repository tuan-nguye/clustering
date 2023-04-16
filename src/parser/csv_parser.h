#include "parser/parser.h"

class Data;

class CSV_Parser: public Parser
{
    public:
        CSV_Parser() {};
        std::vector<Data*> parse();
};