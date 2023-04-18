#include "parser/parser.h"

#ifndef __csv_parser_include__
#define __csv_parser_include__

class CSV_Parser: public Parser
{
    public:
        CSV_Parser() {};
        void parse(std::vector<Data*> &data, std::string file_path);
};

#endif