#include <iostream>
#include <vector>
#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/data.h"

int main()
{
    Parser *parser;
    std::vector<Data*> data;

    // csv test
    CSV_Parser csv_parser;
    parser = &csv_parser;
    parser->parse(data, "./res/iris/iris_data.data");

    
    for(auto it = data.begin(); it != data.end(); it++)
    {
        std::cout << (*it)->to_string() << std::endl;
    }

    std::cout << "number of data objects: " << data.size() << std::endl;

    // ubyte test
    data.clear();
    Ubyte_Parser ubyte_parser;
    parser = &ubyte_parser;
    parser->parse(data, "./res/mnist/t10k-images.idx3-ubyte", "./res/mnist/t10k-labels.idx1-ubyte");

    std::cout << "number of data objects: " << data.size() << std::endl;

    Data *d = data[0];
    for(int i = 0; i < d->attributes.size(); i++)
    {
        if(i % 28 == 0) std::cout << '\n';
        char c = (d->attributes[i] != 0.0) ? char(219) : '.';
        std::cout << c << c;
    }
    std::cout << std::endl << "label: " << d->label << std::endl;

    return 0;
}