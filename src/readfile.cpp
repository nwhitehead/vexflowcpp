#include "readfile.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string read_file(std::string filename) {
    // I hate C++ with a passion
    std::ostringstream buf;
    std::ifstream input{filename};
    if (!input) {
        throw std::runtime_error("Could not open file");
    }
    buf << input.rdbuf();
    return buf.str();
}
