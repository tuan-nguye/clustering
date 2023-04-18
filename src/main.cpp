#include <iostream>
#include <vector>
#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/labeled_data.h"

int main()
{
    Parser *parser;
    std::vector<Data*> data;

    // csv test
    CSV_Parser csv_parser;
    parser = &csv_parser;
    parser->parse(data, "./res/iris/iris_data.data");

    for(Data* d : data)
    {
        std::cout << ((Labeled_Data*)d)->to_string() << std::endl;
    }

    std::cout << "number of data objects: " << data.size() << std::endl;

    // ubyte test
    data.clear();
    Ubyte_Parser ubyte_parser;
    parser = &ubyte_parser;
    parser->parse(data, "./res/mnist/t10k-images.idx3-ubyte", "./res/mnist/t10k-labels.idx1-ubyte");

    std::cout << "number of data objects: " << data.size() << std::endl;

    Labeled_Data *lb = (Labeled_Data*) data[0];
    for(int i = 0; i < lb->attributes.size(); i++)
    {
        if(i % 28 == 0) std::cout << '\n';
        char c = (lb->attributes[i] != 0.0) ? char(219) : '.';
        std::cout << c << c;
    }
    std::cout << std::endl << "label: " << lb->label << std::endl;

    return 0;
}