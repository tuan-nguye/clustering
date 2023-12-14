#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>

#include "data/data.h"
#include "data/structures/maptor.h"


#ifndef __print_result_table__
#define __print_result_table__

class Print_Result_Table
{
    private:
        //static std::string root = std::string("build/out");

        static void pad_left(std::string &str, int length, char c)
        {
            int pad_size = length-str.length();
            if(pad_size <= 0) return;
            str.insert(0, pad_size, c);
        }
        static void pad_centered(std::string &str, int length, char c)
        {
            int diff = length-str.length();
            if(diff <= 0) return;
            int right_pad = diff/2, left_pad = length-right_pad;
            pad_left(str, left_pad, c);
            str.append(right_pad, c);
        }
    public:
        static std::string print(std::unordered_map<Data*, std::string> result_map)
        {
            std::unordered_map<std::string, std::unordered_map<std::string, int>> count_map;
            Maptor<std::string> labels_orig, labels_clus;
            int max_length_orig = 0;
            int max_count = 0;

            for(auto &entry : result_map)
            {
                std::string label_orig = entry.first->label;
                std::string label_clus = entry.second;
                labels_orig.push_back(label_orig);
                labels_clus.push_back(label_clus);
                count_map[label_clus][label_orig]++;

                if(count_map[label_clus][label_orig] > max_count) max_count = count_map[label_clus][label_orig];
                if(label_orig.length() > max_length_orig) max_length_orig = label_orig.length();
            }

            int num_len = int(ceil(log10(max_count)));
            if(num_len > max_length_orig) max_length_orig = num_len;

            std::string out;

            // table config
            char pad_char = ' ', vertical_border = '|', horizontal_border = '-';
            int first_cell_width = 5;
            int cell_width = max_length_orig;
            int table_width = first_cell_width + labels_orig.size()*(cell_width+1);
            std::string row_border, row_border_block;
            row_border_block += '+';
            row_border_block.append(cell_width, horizontal_border);
            for(int i = 0; i < labels_orig.size(); i++) row_border += row_border_block;
            pad_left(row_border, table_width, horizontal_border);
            std::string line;

            // print column headers
            labels_clus.sort();
            labels_orig.sort();

            line.append("label");
            for(std::string label_orig : labels_orig)
            {
                pad_centered(label_orig, cell_width, pad_char);
                line += vertical_border;
                line.append(label_orig);
            }
            out += line + '\n';
            line.clear();

            // print rows

            for(std::string label_clus : labels_clus)
            {
                line.clear();
                for(std::string label_orig : labels_orig)
                {
                    std::string count = std::to_string(count_map[label_clus][label_orig]);
                    pad_centered(count, cell_width, pad_char);
                    line += vertical_border;
                    line += count;
                }
                pad_centered(label_clus, first_cell_width, pad_char);
                line.insert(0, label_clus);
                out += row_border + '\n' + line + '\n';
            }

            // calculate accuracy
            int total = 0, correct = 0;

            for(auto &e1 : count_map)
            {
                std::string label_clus = e1.first;
                int max = 0, sum = 0;
                for(auto &e2 : e1.second)
                {
                    int count = e2.second;
                    if(count > max) max = count;
                    sum += count;
                }
                total += sum;
                correct += max;
            }

            out += "\ncorrect: " + std::to_string(float(correct)/total*100) + '%';
            return out;
        }

        static void write_to_file(std::string &input, std::string file_name)
        {
            std::ofstream file(file_name);

            if(file.is_open())
            {
                file << input;
                file.close();
                std::cout << "done writing file" << std::endl;
            } else
            {
                std::cout << "failed writing file" << std::endl;
            }
        }
};

#endif