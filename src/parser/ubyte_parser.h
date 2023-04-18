#include "parser/parser.h"

#ifndef __ubyte_parser_include__
#define __ubyte_parser_include__

class Ubyte_Parser: public Parser
{
    private:
        int buffer_to_int(char buffer[4]);
    public:
        Ubyte_Parser() {};
        void parse(std::vector<Data*> &data, std::string datafile_path);
        void parse(std::vector<Data*> &data, std::string datafile_path, std::string labelfile_path);
};

#endif