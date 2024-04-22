#include "javascript_runtime.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include "quickjs.h"
#pragma GCC diagnostic pop

#include "stb_truetype.h"
#include "canvas.h"
#include "readfile.h"

namespace {

JSModuleDef *module_loader(JSContext *ctx, const char *module_name, void */*opaque*/) {
    JSModuleDef *m;
    JSValue func_val;
    std::string buf;
    try {
        buf = read_file(module_name);
    } catch(...) {
        JS_ThrowReferenceError(ctx, "could not load module filename '%s'", module_name);
        return nullptr;
    }
    // Compile module
    func_val = JS_Eval(ctx, buf.data(), buf.size(), module_name, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
    if (JS_IsException(func_val)) {
        return nullptr;
    }
    // Module is referenced, so free it once
    m = static_cast<JSModuleDef*>(JS_VALUE_GET_PTR(func_val));
    JS_FreeValue(ctx, func_val);
    return m;
}

std::string get_string(JSContext *ctx, JSValueConst &arg) {
    size_t len{};
    const char *str = JS_ToCStringLen(ctx, &len, arg);
    if (!str) {
        throw std::runtime_error("getString exception");
    }
    std::string msg{str};
    JS_FreeCString(ctx, str);
    return msg;
}

int get_int32(JSContext *ctx, JSValueConst &arg) {
    double value{0};
    if (!JS_IsNumber(arg)) {
        throw std::runtime_error("getInt32 exception, not a number");

    }
    if (JS_ToFloat64(ctx, &value, arg)) {
        throw std::runtime_error("getInt32 exception, could not convert to float64");
    }
    return static_cast<int>(value);
}

double get_float64(JSContext *ctx, JSValueConst &arg) {
    double value{0};
    if (!JS_IsNumber(arg)) {
        throw std::runtime_error("getInt32 exception, not a number");

    }
    if (JS_ToFloat64(ctx, &value, arg)) {
        throw std::runtime_error("getInt32 exception, could not convert to float64");
    }
    return value;
}

} // anonymous namespace

JSValue cpp_draw_character(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 5);
    int character = get_int32(ctx, argv[0]);
    int x = std::round(get_float64(ctx, argv[1]));
    int y = std::round(get_float64(ctx, argv[2]));
    std::string font = get_string(ctx, argv[3]);
    double scale = get_float64(ctx, argv[4]);
    Renderer &renderer = static_cast<JavaScriptRuntime*>(JS_GetContextOpaque(ctx))->get_renderer();
    renderer.draw_character(x, y, character, font, scale);
    return JS_UNDEFINED;
}

JSValue cpp_draw_line(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 4);
    double x0 = get_float64(ctx, argv[0]);
    double y0 = get_float64(ctx, argv[1]);
    double x1 = get_float64(ctx, argv[2]);
    double y1 = get_float64(ctx, argv[3]);
    Renderer &renderer = static_cast<JavaScriptRuntime*>(JS_GetContextOpaque(ctx))->get_renderer();
    renderer.get_canvas().draw_line(x0, y0, x1, y1);
    return JS_UNDEFINED;
}

JSValue cpp_fill_rect(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 4);
    double x = get_float64(ctx, argv[0]);
    double y = get_float64(ctx, argv[1]);
    double w = get_float64(ctx, argv[2]);
    double h = get_float64(ctx, argv[3]);
    Renderer &renderer = static_cast<JavaScriptRuntime*>(JS_GetContextOpaque(ctx))->get_renderer();
    renderer.get_canvas().fill_rect(x, y, w, h);
    return JS_UNDEFINED;
}

JSValue cpp_import_script(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 1);
    std::string filename{get_string(ctx, argv[0])};
    JavaScriptRuntime *this_pointer = static_cast<JavaScriptRuntime*>(JS_GetContextOpaque(ctx));
    std::string contents = read_file(filename);
    //std::cout << "cpp_import_script len=" << contents.size() << " filename=" << filename << " ctx=" << (void*)ctx << " this_pointer->context=" << (void*)this_pointer->context << std::endl;
    this_pointer->_eval(contents, filename, false, false);
    return JS_UNDEFINED;
}

JSValue cpp_get_font_scale(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 2);
    std::string fontname{get_string(ctx, argv[0])};
    double text_height{get_float64(ctx, argv[1])};
    Renderer &renderer = static_cast<JavaScriptRuntime*>(JS_GetContextOpaque(ctx))->get_renderer();
    return JS_NewFloat64(ctx, renderer.get_font_scale(fontname, text_height));
}

JSValue cpp_measure_text(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 3);
    int character = get_int32(ctx, argv[0]);
    std::string font{get_string(ctx, argv[1])};
    double scale{get_float64(ctx, argv[2])};
    std::cout << "cpp_measure_text(" << character << ", " << font << ", " << scale << ")" << std::endl;
    Renderer &renderer = static_cast<JavaScriptRuntime*>(JS_GetContextOpaque(ctx))->get_renderer();
    CodepointMetrics metrics = renderer.measure_character(character, font, scale);
    JSValue result = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, result, "width", JS_NewFloat64(ctx, metrics.width));
    return result;
}

JSValue cpp_print(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 1);
    std::string msg{get_string(ctx, argv[0])};
    std::cout << msg << std::endl;
    return JS_UNDEFINED;
}

JSValue cpp_read_file(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 1);
    std::string filename{get_string(ctx, argv[0])};
    std::string contents = read_file(filename);
    return JS_NewStringLen(ctx, contents.c_str(), contents.size());
}

JSValue cpp_register_font(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 2);
    std::string filename{get_string(ctx, argv[0])};
    std::string fontname{get_string(ctx, argv[1])};
    Renderer &renderer = static_cast<JavaScriptRuntime*>(JS_GetContextOpaque(ctx))->get_renderer();
    renderer.register_font(filename, fontname);
    return JS_UNDEFINED;
}

Renderer &JavaScriptRuntime::get_renderer() {
    return renderer;
}

JavaScriptRuntime::JavaScriptRuntime() {
    // Create runtime and context
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
    // Opaque data for context is "this", so we can look it up from non-member functions that only have context
    JS_SetContextOpaque(context, this);
    // Register module loader
    JS_SetModuleLoaderFunc(runtime, nullptr, module_loader, nullptr);
    // Register globals
    JSValue global = JS_GetGlobalObject(context);
    JS_SetPropertyStr(context, global, "cpp_draw_character", JS_NewCFunction(context, cpp_draw_character, "cpp_draw_character", 5));
    JS_SetPropertyStr(context, global, "cpp_draw_line", JS_NewCFunction(context, cpp_draw_line, "cpp_draw_line", 4));
    JS_SetPropertyStr(context, global, "cpp_fill_rect", JS_NewCFunction(context, cpp_fill_rect, "cpp_fill_rect", 4));
    JS_SetPropertyStr(context, global, "cpp_get_font_scale", JS_NewCFunction(context, cpp_get_font_scale, "cpp_get_font_scale", 2));
    JS_SetPropertyStr(context, global, "cpp_import_script", JS_NewCFunction(context, cpp_import_script, "cpp_import_script", 1));
    JS_SetPropertyStr(context, global, "cpp_measure_text", JS_NewCFunction(context, cpp_measure_text, "cpp_measure_text", 3));
    JS_SetPropertyStr(context, global, "cpp_print", JS_NewCFunction(context, cpp_print, "cpp_print", 1));
    JS_SetPropertyStr(context, global, "cpp_read_file", JS_NewCFunction(context, cpp_read_file, "cpp_read_file", 1));
    JS_SetPropertyStr(context, global, "cpp_register_font", JS_NewCFunction(context, cpp_register_font, "cpp_register_font", 2));
    JS_FreeValue(context, global);
}

JavaScriptRuntime::~JavaScriptRuntime() {
    JS_FreeContext(context);
    JS_FreeRuntime(runtime);
}

void JavaScriptRuntime::eval(std::string code, std::string source_filename) {
    std::lock_guard<std::mutex> guard(mutex);
    _eval(code, source_filename, false, false);
}

void JavaScriptRuntime::eval_module(std::string code, std::string source_filename) {
    std::lock_guard<std::mutex> guard(mutex);
    _eval(code, source_filename, true, false);
}

void JavaScriptRuntime::eval_module_await(std::string code, std::string source_filename) {
    std::lock_guard<std::mutex> guard(mutex);
    _eval(code, source_filename, true, true);
}

// Wait for a promise and execute pending jobs while waiting for it. Return the
// promise result or JS_EXCEPTION in case of promise rejection.
JSValue std_await(JSContext *ctx, JSValue obj)
{
    JSValue ret;
    int state;

    while(true) {
        state = JS_PromiseState(ctx, obj);
        if (state == JS_PROMISE_FULFILLED) {
            ret = JS_PromiseResult(ctx, obj);
            JS_FreeValue(ctx, obj);
            break;
        } else if (state == JS_PROMISE_REJECTED) {
            ret = JS_Throw(ctx, JS_PromiseResult(ctx, obj));
            JS_FreeValue(ctx, obj);
            break;
        } else if (state == JS_PROMISE_PENDING) {
            JSContext *ctx1;
            int err;
            err = JS_ExecutePendingJob(JS_GetRuntime(ctx), &ctx1);
            if (err < 0) {
                std::cerr << "JS_ExecutePendingJob failed" << std::endl;
            }
        } else {
            ret = obj;
            break;
        }
    }
    return ret;
}

void JavaScriptRuntime::_eval(std::string code, std::string source_filename, bool is_module, bool await) {
    //std::cout << "Entering _eval for " << source_filename << std::endl;
    JSValue val = JS_Eval(context, code.c_str(), code.size(), source_filename.c_str(), is_module ? JS_EVAL_TYPE_MODULE : 0);
    //std::cout << "Done _eval for " << source_filename << std::endl;
    if (await) {
        val = std_await(context, val);
    }
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

void JavaScriptRuntime::set(std::string identifier, std::string value) {
    JSValue global = JS_GetGlobalObject(context);
    JS_SetPropertyStr(context, global, identifier.c_str(), JS_NewStringLen(context, value.data(), value.size()));
    JS_FreeValue(context, global);
}

void JavaScriptRuntime::save(std::string filename) {
    renderer.get_canvas().save(filename);
}
