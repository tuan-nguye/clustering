#include <fstream>
#include "parser/ubyte_parser.h"

void Ubyte_Parser::parse(std::vector<Data*> &data, std::string file_path)
{
    std::ifstream file(file_path, std::ios::binary);  // open the file for reading in binary mode
    if (!file.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return;
    }

    char buffer[4];
    file.read(buffer, 4);  // read the magic number (4 bytes)
    int magic_number = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    std::cout << "Magic number: " << magic_number << std::endl;

    file.read(buffer, 4);  // read the number of images (4 bytes)
    int num_images = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    std::cout << "Number of images: " << num_images << std::endl;

    file.read(buffer, 4);  // read the number of rows (4 bytes)
    int num_rows = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    std::cout << "Number of rows: " << num_rows << std::endl;

    file.read(buffer, 4);  // read the number of columns (4 bytes)
    int num_cols = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    std::cout << "Number of columns: " << num_cols << std::endl;

    // read the image data
    for (int i = 0; i < num_images; ++i) {
        std::cout << "Image " << i + 1 << ":" << std::endl;
        for (int j = 0; j < num_rows * num_cols; ++j) {
            unsigned char pixel;
            file.read((char*)&pixel, 1);  // read one byte at a time
            std::cout << (int)pixel << " ";  // print the pixel value
        }
        std::cout << std::endl;
    }

    file.close();  // don't forget to close the file
};