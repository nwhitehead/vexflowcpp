
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "javascript_runtime.h"
#include "readfile.h"

extern unsigned char external_opensheetmusicdisplay_js[];
extern int external_opensheetmusicdisplay_js_len;
extern unsigned char src_vexflow_wrap_mjs[];
extern int src_vexflow_wrap_mjs_len;

int main(int argc, char *argv[]) {
    std::cout << "Offline C++ VexFlow render" << std::endl;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " JSFILE" << std::endl;
        return EXIT_FAILURE;
    }
    std::string filename{argv[1]};
    JavaScriptRuntime runtime;
    runtime.eval_module_await(read_file(filename), filename);
    runtime.save("out.png");
    return EXIT_SUCCESS;
}
