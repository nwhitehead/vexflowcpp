#include "javascript_runtime.h"

#include <cassert>
#include <iostream>
#include <map>
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
    void blit(int x, int y, uint8_t *src, int w, int h) {
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                set(x + j, y + i, src[j + i * w]);
            }
        }
    }
    void save(std::string filename) {
        stbi_write_png(filename.c_str(), width, height, 1, data.data(), width);
    }
};

class Renderer {
private:
    Canvas canvas;
    // Font to use for drawing text/symbols
    stbtt_fontinfo font;
    float size;
    float scale_height;
public:
    Renderer() : canvas(800, 600) {
        int success = stbtt_InitFont(&font, external_Bravura_otf, stbtt_GetFontOffsetForIndex(external_Bravura_otf, 0));
        if (!success) {
            throw std::runtime_error("Bravura font could not be initialized");
        }
        size = 430.0f;
        scale_height = stbtt_ScaleForPixelHeight(&font, size);
    }
    ~Renderer() {
    }
    void drawCharacter(int x, int y, int character) {
        int w, h, xoff, yoff;
        unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, 0, scale_height, character, &w, &h, &xoff, &yoff);
        canvas.blit(x, y, bitmap, w, h);
        std::free(bitmap);
    }
    Canvas &get_canvas() {
        return canvas;
    }
};

// YUCK globals, can't figure out how to get pointer to context for C callbacks
Renderer renderer{};

std::string getString(JSContext *ctx, JSValueConst &arg) {
    size_t len{};
    const char *str = JS_ToCStringLen(ctx, &len, arg);
    if (!str) {
        throw std::runtime_error("getString exception");
    }
    std::string msg{str};
    JS_FreeCString(ctx, str);
    return msg;
}

int getInt32(JSContext *ctx, JSValueConst &arg) {
    double value{0};
    if (!JS_IsNumber(arg)) {
        throw std::runtime_error("getInt32 exception, not a number");

    }
    if (JS_ToFloat64(ctx, &value, arg)) {
        throw std::runtime_error("getInt32 exception, could not convert to float64");
    }
    return static_cast<int>(value);
}

JSValue cpp_print(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 1);
    std::string msg{getString(ctx, argv[0])};
    std::cout << msg << std::endl;
    return JS_UNDEFINED;
}

JSValue cpp_draw_character(JSContext *ctx, JSValueConst /*this_val*/, int argc, JSValueConst *argv) {
    assert(argc == 3);
    int character = getInt32(ctx, argv[0]);
    int x = getInt32(ctx, argv[1]);
    int y = getInt32(ctx, argv[2]);
    renderer.drawCharacter(x, y, character);
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
    JS_SetPropertyStr(context, global, "cpp_draw_character", JS_NewCFunction(context, cpp_draw_character, "cpp_draw_character", 3));
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

void JavaScriptRuntime::save(std::string filename) {
    renderer.get_canvas().save("out.png");
}
