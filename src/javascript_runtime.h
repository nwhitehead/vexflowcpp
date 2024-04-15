#pragma once

#include <mutex>
#include <string>

struct JSRuntime;
struct JSContext;

class JavaScriptRuntime {

private:
    std::mutex mutex;
    JSRuntime *runtime;
    JSContext *context;

public:
    JavaScriptRuntime();
    ~JavaScriptRuntime();

    // Eval JavaScript code
    void eval(std::string code, std::string source_filename);

    // Set global variable to a string (to get data into runtime)
    void set(std::string identifier, std::string value);

    // Save graphical output
    void save(std::string filename);
};
