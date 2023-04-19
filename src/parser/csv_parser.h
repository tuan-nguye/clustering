#include "parser/parser.h"

#ifndef __csv_parser_include__
#define __csv_parser_include__

class CSV_Parser: public Parser
{
    public:
        void parse(std::vector<Data> &data, std::string datafile_path);
        void parse(std::vector<Data> &data, std::string datafile_path, std::string labelfile_path);
};

#endif