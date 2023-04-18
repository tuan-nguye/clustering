#include <iostream>
#include <vector>
#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/labeled_data.h"

int main()
{
    CSV_Parser csv_parser;
    std::vector<Data*> data;
    csv_parser.parse(data, "./res/iris/iris_data.data");

    for(Data* d : data)
    {
        std::cout << ((Labeled_Data*)d)->to_string() << std::endl;
    }

    std::cout << "number of data objects: " << data.size() << std::endl;

    Ubyte_Parser ubyte_parser;
    data.clear();
    ubyte_parser.parse(data, "./res/mnist/t10k-images.idx3-ubyte");

    return 0;
}