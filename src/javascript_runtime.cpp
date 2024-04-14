#include "javascript_runtime.h"

#include <cassert>
#include <iostream>
#include <map>
#include <stdexcept>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include "quickjs.h"
#pragma GCC diagnostic pop

std::string getString(JSContext *ctx, JSValueConst &arg) {
    size_t len{};
    const char *str = JS_ToCStringLen(ctx, &len, arg);
    if (!str) {
        throw std::runtime_error("js_getString exception");
    }
    std::string msg{str};
    JS_FreeCString(ctx, str);
    return msg;
}

JSValue cpp_print(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 1);
    std::string msg{getString(ctx, argv[0])};
    std::cout << msg << std::endl;
    return JS_UNDEFINED;
}

JSValue cpp_measure_text(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 2);
    std::string txt{getString(ctx, argv[0])};
    std::string font{getString(ctx, argv[1])};
    std::cout << "cpp_measure_text(" << txt << ", " << font << ")" << std::endl;
    return JS_UNDEFINED;
}

JavaScriptRuntime::JavaScriptRuntime() {
    runtime = JS_NewRuntime();
    if (!runtime) {
        std::cerr << "JavaScriptRuntime::JavaScriptRuntime() Could not create QuickJS runtime" << std::endl;
        throw new std::runtime_error("Could not create QuickJS runtime");
    }
    context = JS_NewContext(runtime);
    if (!context) {
        std::cerr << "JavaScriptRuntime::JavaScriptRuntime() Could not create QuickJS context" << std::endl;
        throw new std::runtime_error("Could not create QuickJS runtime");
    }
    // Register globals
    JSValue global = JS_GetGlobalObject(context);
    JS_SetPropertyStr(context, global, "cpp_print", JS_NewCFunction(context, cpp_print, "cpp_print", 1));
    JS_SetPropertyStr(context, global, "cpp_measure_text", JS_NewCFunction(context, cpp_measure_text, "cpp_measure_text", 2));
    JS_FreeValue(context, global);
}

JavaScriptRuntime::~JavaScriptRuntime() {
    JS_FreeContext(context);
    JS_FreeRuntime(runtime);
}

void JavaScriptRuntime::eval(std::string code, std::string source_filename) {
    std::lock_guard<std::mutex> guard(mutex);
    JSValue val = JS_Eval(context, code.c_str(), code.size(), source_filename.c_str(), 0);
    // Check if return value is an exception
    if (JS_IsException(val)) {
        JSValue exval = JS_GetException(context);
        // Try to be similar to how Node shows exceptions
        // First look for string conversion
        const char *txt = JS_ToCString(context, exval);
        if (txt) {
            std::cerr << "JavaScript: Uncaught " << txt << std::endl;
            JS_FreeCString(context, txt);
        } else {
            std::cerr << "JavaScript: Uncaught exception [[exception]]" << std::endl;
        }
        // Try to show stack trace
        // Only works if exception is descended from Error
        if (JS_IsError(context, exval)) {
            JSValue v = JS_GetPropertyStr(context, exval, "stack");
            if (!JS_IsUndefined(v)) {
                const char *str = JS_ToCString(context, v);
                if (str) {
                    std::cerr << str << std::endl;
                    JS_FreeCString(context, str);
                }
            }
            JS_FreeValue(context, v);
        }
        JS_FreeValue(context, exval);
    }
    JS_FreeValue(context, val);
}

std::string JavaScriptRuntime::get(std::string name)
{
    std::lock_guard<std::mutex> guard(mutex);
    std::string result;
    JSValue global_object = JS_GetGlobalObject(context);
    JSValue v = JS_GetPropertyStr(context, global_object, name.c_str());
    if (!JS_IsUndefined(v)) {
        const char *str = JS_ToCString(context, v);
        if (str) {
            result = std::string{str};
            JS_FreeCString(context, str);
        }
    }
    JS_FreeValue(context, v);
    JS_FreeValue(context, global_object);
    return result;
}
