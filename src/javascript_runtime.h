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

    // Save graphical output
    void save(std::string filename);
};
