#include "parser/parser.h"

#ifndef __ubyte_parser_include__
#define __ubyte_parser_include__

class Ubyte_Parser: public Parser
{
    public:
        Ubyte_Parser() {};
        void parse(std::vector<Data*> &data, std::string file_path);
};

#endif