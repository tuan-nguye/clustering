#include <iostream>
#include "data/labeled_data.h"
#include "parser/csv_parser.h"

int main()
{
    CSV_Parser csv_parser;
    csv_parser.parse();
    Labeled_Data ldata("Fugma S. Haad");
    std::cout << ldata.label << std::endl;
    std::cout << "Mike Oxlarge" << std::endl;
    return 0;
}