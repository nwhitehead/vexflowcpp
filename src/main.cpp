
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "javascript_runtime.h"
#include "readfile.h"

int main(int argc, char *argv[]) {
    std::cout << "Offline C++ VexFlow render" << std::endl;

    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " JSFILE ARG" << std::endl;
        return EXIT_FAILURE;
    }
    std::string filename{argv[1]};
    JavaScriptRuntime runtime;
    runtime.set("arg", argv[2]);
    runtime.eval_module_await(read_file(filename), filename);
    runtime.save("out.png");
    return EXIT_SUCCESS;
}
