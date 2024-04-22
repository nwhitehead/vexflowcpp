
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "javascript_runtime.h"

extern unsigned char external_opensheetmusicdisplay_js[];
extern int external_opensheetmusicdisplay_js_len;
extern unsigned char src_vexflow_wrap_mjs[];
extern int src_vexflow_wrap_mjs_len;

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

int main(int argc, char *argv[]) {
    std::cout << "Offline C++ VexFlow render" << std::endl;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " JSFILE" << std::endl;
        return EXIT_FAILURE;
    }
    std::string filename{argv[1]};
    JavaScriptRuntime runtime;
    runtime.eval_await(read_file(filename), filename);
    return EXIT_SUCCESS;
}
