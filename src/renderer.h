
#pragma once

#include <string>
#include <map>
#include <memory>

#include "canvas.h"
#include "stb_truetype.h"
#include "readfile.h"

struct CodepointMetrics {
    double width;
    double actualBoundingBoxLeft;
    double actualBoundingBoxRight;
    double fontBoundingBoxAscent;
    double fontBoundingBoxDescent;
    double actualBoundingBoxAscent;
    double actualBoundingBoxDescent;
};

class Font {
public:
    stbtt_fontinfo font;
    std::vector<uint8_t> data;
    Font(std::string filename) {
        std::string contents = read_file(filename);
        data = std::vector<uint8_t>(contents.begin(), contents.end());
        int success = stbtt_InitFont(&font, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0));
        if (!success) {
            throw std::runtime_error("Font could not be initialized");
        }
    };
    ~Font() {};
};

class Renderer {
private:
    Canvas canvas;
    // Font to use for drawing text/symbols
    std::map<std::string, Font> fonts;
    stbtt_fontinfo *get_font(std::string);
public:
    Renderer();
    ~Renderer();
    void register_font(std::string filename, std::string fontname);
    void draw_character(int x, int y, int character, std::string fontname, double scale);
    CodepointMetrics measure_character(int character, std::string fontname, double scale);
    Canvas &get_canvas();
    double get_font_scale(std::string fontname, double pixel_height);
};
