#include <ctime>

#include "util/data_generator.h"

std::random_device Data_Generator::rd{};
std::mt19937 Data_Generator::gen(static_cast<unsigned int>(time(nullptr)));//gen{rd()};