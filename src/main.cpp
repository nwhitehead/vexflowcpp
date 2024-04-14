
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "javascript_runtime.h"

extern unsigned char external_vexflow_debug_js_js[];
extern int external_vexflow_debug_js_js_len;
extern unsigned char src_vexflow_wrap_js[];
extern int src_vexflow_wrap_js_len;

std::string read_file(std::string filename) {
    // I hate C++ with a passion
    std::ostringstream buf;
    std::ifstream input{filename};
    buf << input.rdbuf();
    return buf.str();
}

int main(int argc, char *argv[]) {
    std::cout << "Offline C++ VexFlow render" << std::endl;

    JavaScriptRuntime runtime;
    runtime.eval(std::string((char *)external_vexflow_debug_js_js), "vexflow-debug.js");
    runtime.eval(std::string((char *)src_vexflow_wrap_js, src_vexflow_wrap_js_len), "vexflow_wrap.js");
    runtime.save("out.png");
    return EXIT_SUCCESS;
}
