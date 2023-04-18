#include "parser/csv_parser.h"
#include "data/labeled_data.h"
#include <fstream>

void CSV_Parser::parse(std::vector<Data*> &data, std::string file_path)
{
    std::cout << "CSV_Parser" << std::endl;
    std::ifstream file(file_path);

    if(!file.is_open())
    {
        std::cout << "failed to open file" << std::endl;
        return;
    }

    std::string delimiter = ",";
    std::string line;
    while(std::getline(file, line))
    {
        if(line.empty()) continue;
        
        //std::cout << line << std::endl;
        std::vector<std::string> vals;

        int i = 0;
        while((i = line.find(delimiter)) != std::string::npos)
        {
            std::string attribute = line.substr(0, i);
            vals.push_back(attribute);
            line.erase(0, i+delimiter.size());
        }
        vals.push_back(line);

        Labeled_Data *ld = new Labeled_Data(vals.back());
        vals.pop_back();

        for(std::string v : vals)
        {
            ld->attributes.push_back(std::stof(v));
        }

        data.push_back(ld);
    }
};