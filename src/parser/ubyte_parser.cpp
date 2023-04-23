#include <fstream>

#include "parser/ubyte_parser.h"
#include "data/data.h"

void Ubyte_Parser::parse(std::vector<Data*> &data, std::string datafile_path)
{
    // no implementation here
}

void Ubyte_Parser::parse(std::vector<Data*> &data, std::string datafile_path, std::string labelfile_path)
{
    std::ifstream datafile(datafile_path, std::ios::binary);  // open the file for reading in binary mode
    std::ifstream labelfile(labelfile_path, std::ios::binary);
    
    if (!datafile.is_open() || !labelfile.is_open()) {
        std::cerr << "Failed to open files!" << std::endl;
        return;
    }

    char buffer[4];
    datafile.read(buffer, 4);  // read the magic number (4 bytes)
    int magic_number_data = buffer_to_int(buffer);
    labelfile.read(buffer, 4);
    int magic_number_label = buffer_to_int(buffer);
    std::cout << "Magic number data: " << magic_number_data << std::endl;
    std::cout << "Magic number label: " << magic_number_label << std::endl;

    datafile.read(buffer, 4);  // read the number of images (4 bytes)
    int num_images = buffer_to_int(buffer);
    labelfile.read(buffer, 4);
    int num_labels = buffer_to_int(buffer);
    std::cout << "Number of images: " << num_images << std::endl;
    std::cout << "Number of labels: " << num_labels << std::endl;

    datafile.read(buffer, 4);  // read the number of rows (4 bytes)
    int num_rows = buffer_to_int(buffer);
    std::cout << "Number of rows: " << num_rows << std::endl;

    datafile.read(buffer, 4);  // read the number of columns (4 bytes)
    int num_cols = buffer_to_int(buffer);
    std::cout << "Number of columns: " << num_cols << std::endl;

    if(num_images != num_labels)
    {
        std::cout << "number of images and labels don't match" << std::endl;
        return;
    }

    // read the image data
    for (int i = 0; i < num_images; ++i) {
        //std::cout << "Image " << i + 1 << ":" << std::endl;
        unsigned char c;
        labelfile.read((char*) &c, 1);
        std::string label = std::to_string(c);
        Data *d = new Data(label);

        for (int j = 0; j < num_rows * num_cols; ++j) {
            datafile.read((char*) &c, 1);  // read one byte at a time
            float f = float(c);
            d->push_back(f);
        }
        
        data.push_back(d);
    }

    datafile.close();  // don't forget to close the file
}

int Ubyte_Parser::buffer_to_int(char buffer[4])
{
    int num = 0;

    for(int i = 0; i < 4; i++)
    {
        num <<= 8;
        num |= (unsigned char) buffer[i];
    }

    return num;
}