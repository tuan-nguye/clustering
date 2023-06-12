#include <unordered_map>
#include <string>
#include <iostream>

#include "data/data.h"
#include "data/structures/maptor.h"


#ifndef __print_result_table__
#define __print_result_table__

class Print_Result_Table
{
    private:
        static void pad(std::string &str, int length, char c)
        {
            int pad_size = length-str.length();
            if(pad_size <= 0) return;
            str.insert(0, pad_size, c);
        }
    public:
        static void print(std::unordered_map<Data*, std::string> result_map)
        {
            std::unordered_map<std::string, std::unordered_map<std::string, int>> count_map;
            Maptor<std::string> labels_orig, labels_clus;
            int max_length_orig = 0;

            for(auto &entry : result_map)
            {
                std::string label_orig = entry.first->label;
                std::string label_clus = entry.second;
                labels_orig.push_back(label_orig);
                labels_clus.push_back(label_clus);
                count_map[label_orig][label_clus]++;

                if(label_orig.length() > max_length_orig) max_length_orig = label_orig.length();
            }

            // table config
            char pad_char = ' ', vertical_border = '|', horizontal_border = '-';
            int first_cell_width = max_length_orig;
            int cell_width = 5;
            int table_width = first_cell_width + labels_clus.size()*(cell_width+1);
            std::string row_border, row_border_block;
            row_border_block += '+';
            row_border_block.append(cell_width, horizontal_border);
            for(int i = 0; i < labels_clus.size(); i++) row_border += row_border_block;
            pad(row_border, table_width, horizontal_border);
            std::string line;

            labels_clus.sort();

            // print column headers
            line.append(first_cell_width, pad_char);
            for(std::string label_clus : labels_clus)
            {
                pad(label_clus, cell_width, pad_char);
                line += vertical_border;
                line.append(label_clus);
            }
            std::cout << line << std::endl;
            line.clear();

            // print rows

            for(std::string label_orig : labels_orig)
            {
                line.clear();
                for(std::string label_clus : labels_clus)
                {
                    std::string count = std::to_string(count_map[label_orig][label_clus]);
                    pad(count, cell_width, pad_char);
                    line += vertical_border;
                    line += count;
                }
                pad(label_orig, first_cell_width, pad_char);
                line.insert(0, label_orig);
                std::cout << row_border << '\n' << line << std::endl;
            }
        }
};

#endif