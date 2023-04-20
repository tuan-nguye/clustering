#include <iostream>
#include <vector>
#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/data.h"
#include "algorithm/greedy_joining.h"

int main()
{
    Parser *parser;
    std::vector<Data*> data;

    // csv test
    CSV_Parser csv_parser;
    parser = &csv_parser;
    parser->parse(data, "./res/iris/iris_data.data");
    std::cout << "number of data objects: " << data.size() << std::endl;

    Greedy_Joining gr_joining;
    Clustering *clustering = &gr_joining;

    double d = 2.5;
    std::map<Data*, int> clustering_result = clustering->execute(data, d);

    for(auto &entry : clustering_result)
    {
        std::cout << "cl_label: " << entry.second << ", " << entry.first->to_string() << std::endl;
    }

    return 0;
}