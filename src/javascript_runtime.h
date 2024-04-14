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

    // Get JSON string from environment
    std::string get(std::string name);
};
