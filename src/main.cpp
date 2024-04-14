
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "javascript_runtime.h"
#include "stb_image_write.h"
#include "stb_truetype.h"

extern unsigned char external_Bravura_otf[];
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

int main(int argc, char *argv[]) {
    std::cout << "Offline C++ VexFlow render" << std::endl;

    JavaScriptRuntime runtime;
    runtime.eval(std::string((char *)external_vexflow_debug_js_js), "vexflow-debug.js");
    runtime.eval(std::string((char *)src_vexflow_wrap_js, src_vexflow_wrap_js_len), "vexflow_wrap.js");

    std::string result = runtime.get("__result");
    std::cout << result << std::endl;

    Renderer renderer{};
    int c = 0xE050; // G clef
    renderer.drawCharacter(0, 0, c);
    renderer.get_canvas().save("out.png");
    return EXIT_SUCCESS;
}
