#pragma once

#include <mutex>
#include <string>

#include "renderer.h"

struct JSRuntime;
struct JSContext;

class JavaScriptRuntime {

private:
    std::mutex mutex;
    JSRuntime *runtime;
public:
    JSContext *context;
    Renderer renderer;

    JavaScriptRuntime();
    ~JavaScriptRuntime();

    Renderer &get_renderer();

    // Eval JavaScript code
    void _eval(std::string code, std::string source_filename, bool is_module, bool await);
    void eval(std::string code, std::string source_filename);
    void eval_module(std::string code, std::string source_filename);
    void eval_await(std::string code, std::string source_filename);

    // Set global variable to a string (to get data into runtime)
    void set(std::string identifier, std::string value);

    // Save graphical output
    void save(std::string filename);
};
