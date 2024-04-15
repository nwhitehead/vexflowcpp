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

#include "stb_image_write.h"
#include "stb_truetype.h"

extern unsigned char external_Bravura_otf[];

class Canvas {
private:
    // Dimensions of canvas in pixels
    int width;
    int height;
    // Canvas bitmap data, 0 to 255 per pixel.
    // Values are intensity of what we have drawn, 0 is nothing (transparent to paper), 255 is pure color (black if black on white paper)
    std::vector<uint8_t> data;
public:
    Canvas(int width_p, int height_p) : width(width_p), height(height_p), data(width * height) {
    }
    ~Canvas() {}
    inline int index(int x, int y) {
        return x + width * y;
    }
    inline void set(int x, int y, uint8_t value) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            data[index(x, y)] = value;
        }
    }
    inline void blend(int x, int y, uint8_t value) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            data[index(x, y)] = std::min(255, data[index(x, y)] + value);
        }
    }
    void blit(int x, int y, uint8_t *src, int w, int h) {
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                int value = src[j + i * w];
                    blend(x + j, y + i, value);
            }
        }
    }
    void fill_rect(double x, double y, double w, double h) {
        int xi = std::round(x);
        int yi = std::round(y);
        int wi = std::round(w);
        int hi = std::round(h);
        for (int i = 0; i < hi; i++) {
            for (int j = 0; j < wi; j++) {
                set(xi + j, yi + i, 255);
            }
        }
    }
    void draw_line(double x0, double y0, double x1, double y1) {
        // Code based on description at:
        // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
        int xi0 = std::round(x0);
        int yi0 = std::round(y0);
        int xi1 = std::round(x1);
        int yi1 = std::round(y1);
        int dx = std::abs(xi1 - xi0);
        int sx = xi0 < xi1 ? 1 : -1;
        int dy = -std::abs(yi1 - yi0);
        int sy = yi0 < yi1 ? 1 : -1;
        int error = dx + dy;
        while (true) {
            set(xi0, yi0, 255);
            if (xi0 == xi1 && yi0 == yi1) {
                break;
            }
            int e2 = error * 2;
            if (e2 >= dy) {
                if (xi0 == xi1) {
                    break;
                }
                error += dy;
                xi0 += sx;
            }
            if (e2 <= dx) {
                if (yi0 == yi1) {
                    break;
                }
                error += dx;
                yi0 += sy;
            }
        }
    }
    void save(std::string filename) {
        stbi_write_png(filename.c_str(), width, height, 1, data.data(), width);
    }
};

struct CodepointMetrics {
    double width;
    double actualBoundingBoxLeft;
    double actualBoundingBoxRight;
    double fontBoundingBoxAscent;
    double fontBoundingBoxDescent;
    double actualBoundingBoxAscent;
    double actualBoundingBoxDescent;
};

class Renderer {
private:
    Canvas canvas;
    // Font to use for drawing text/symbols
    stbtt_fontinfo font;
    float size;
    float scale;
public:
    Renderer() : canvas(800, 600) {
        int success = stbtt_InitFont(&font, external_Bravura_otf, stbtt_GetFontOffsetForIndex(external_Bravura_otf, 0));
        if (!success) {
            throw std::runtime_error("Bravura font could not be initialized");
        }
        size = 150.0;
        scale = stbtt_ScaleForPixelHeight(&font, size);
        std::cout << "SCALE=" << scale << std::endl;
    }
    ~Renderer() {
    }
    void draw_character(int x, int y, int character) {
        int w, h, xoff, yoff;
        unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, character, &w, &h, &xoff, &yoff);
        canvas.blit(x + xoff, y + yoff, bitmap, w, h);
        std::free(bitmap);
    }
    CodepointMetrics measure_character(int character) {
        CodepointMetrics result;
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&font, character, &advanceWidth, &leftSideBearing);
        result.width = (advanceWidth - leftSideBearing) * scale;
        return result;
    }
    Canvas &get_canvas() {
        return canvas;
    }
};

// YUCK globals, can't figure out how to get pointer to context for C callbacks
Renderer renderer{};

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

JSValue cpp_print(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 1);
    std::string msg{get_string(ctx, argv[0])};
    std::cout << msg << std::endl;
    return JS_UNDEFINED;
}

JSValue cpp_draw_character(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 3);
    int character = get_int32(ctx, argv[0]);
    int x = std::round(get_float64(ctx, argv[1]));
    int y = std::round(get_float64(ctx, argv[2]));
    renderer.draw_character(x, y, character);
    return JS_UNDEFINED;
}

JSValue cpp_draw_line(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 4);
    double x0 = get_float64(ctx, argv[0]);
    double y0 = get_float64(ctx, argv[1]);
    double x1 = get_float64(ctx, argv[2]);
    double y1 = get_float64(ctx, argv[3]);
    renderer.get_canvas().draw_line(x0, y0, x1, y1);
    return JS_UNDEFINED;
}

JSValue cpp_fill_rect(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 4);
    double x = get_float64(ctx, argv[0]);
    double y = get_float64(ctx, argv[1]);
    double w = get_float64(ctx, argv[2]);
    double h = get_float64(ctx, argv[3]);
    renderer.get_canvas().fill_rect(x, y, w, h);
    return JS_UNDEFINED;
}

JSValue cpp_measure_text(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 2);
    int character = get_int32(ctx, argv[0]);
    std::string font{get_string(ctx, argv[1])};
    std::cout << "cpp_measure_text(" << character << ", " << font << ")" << std::endl;
    CodepointMetrics metrics = renderer.measure_character(character);
    JSValue result = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, result, "width", JS_NewFloat64(ctx, metrics.width));
    return result;
}


namespace {

std::string read_file(std::string filename) {
    // I hate C++ with a passion
    std::ostringstream buf;
    std::ifstream input{filename};
    buf << input.rdbuf();
    return buf.str();
}

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

} // namespace anonymous


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
    // Register module loader
    JS_SetModuleLoaderFunc(runtime, nullptr, module_loader, nullptr);
    // Register globals
    JSValue global = JS_GetGlobalObject(context);
    JS_SetPropertyStr(context, global, "cpp_print", JS_NewCFunction(context, cpp_print, "cpp_print", 1));
    JS_SetPropertyStr(context, global, "cpp_measure_text", JS_NewCFunction(context, cpp_measure_text, "cpp_measure_text", 2));
    JS_SetPropertyStr(context, global, "cpp_draw_character", JS_NewCFunction(context, cpp_draw_character, "cpp_draw_character", 3));
    JS_SetPropertyStr(context, global, "cpp_draw_line", JS_NewCFunction(context, cpp_draw_line, "cpp_draw_line", 4));
    JS_SetPropertyStr(context, global, "cpp_fill_rect", JS_NewCFunction(context, cpp_fill_rect, "cpp_fill_rect", 4));
    JS_FreeValue(context, global);
}

JavaScriptRuntime::~JavaScriptRuntime() {
    JS_FreeContext(context);
    JS_FreeRuntime(runtime);
}

void JavaScriptRuntime::eval(std::string code, std::string source_filename) {
    _eval(code, source_filename, false, false);
}

void JavaScriptRuntime::eval_module(std::string code, std::string source_filename) {
    _eval(code, source_filename, true, false);
}

void JavaScriptRuntime::eval_await(std::string code, std::string source_filename) {
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
    std::lock_guard<std::mutex> guard(mutex);
    JSValue val = JS_Eval(context, code.c_str(), code.size(), source_filename.c_str(), is_module ? JS_EVAL_TYPE_MODULE : 0);
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
