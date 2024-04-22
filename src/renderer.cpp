#include "renderer.h"

#include <iostream>
#include <stdexcept>

#include "readfile.h"

extern unsigned char external_EBGaramond_VariableFont_wght_ttf[];

Renderer::Renderer() : canvas(800, 600) {
}
Renderer::~Renderer() {
}

stbtt_fontinfo *Renderer::get_font(std::string fontname) {
    auto result = fonts.find(fontname);
    if (result == fonts.end()) {
        std::cerr << "font=" << fontname << std::endl;
        throw std::runtime_error("Font not found");
    }
    return &(result->second).font;
}

double Renderer::get_font_scale(std::string fontname, double pixel_height) {
    stbtt_fontinfo *f = get_font(fontname);
    return stbtt_ScaleForPixelHeight(f, pixel_height);
    // // Alternative measure only using ascent:
    // int ascent, descent, lineGap;
    // stbtt_GetFontVMetrics(f, &ascent, &descent, &lineGap);
    // return pixel_height / ascent;
}

void Renderer::register_font(std::string filename, std::string fontname) {
    fonts.try_emplace(fontname, filename);
}

void Renderer::draw_character(int x, int y, int character, std::string fontname, double scale) {
    int w, h, xoff, yoff;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(get_font(fontname), 0, scale, character, &w, &h, &xoff, &yoff);
    canvas.blit(x + xoff, y + yoff, bitmap, w, h);
    std::free(bitmap);
}

CodepointMetrics Renderer::measure_character(int character, std::string fontname, double scale) {
    CodepointMetrics result;
    stbtt_fontinfo *f = get_font(fontname);
    int advanceWidth, leftSideBearing;
    int ascent, descent, lineGap;
    int x0, y0, x1, y1;
    stbtt_GetCodepointHMetrics(f, character, &advanceWidth, &leftSideBearing);
    stbtt_GetFontVMetrics(f, &ascent, &descent, &lineGap);
    stbtt_GetCodepointBox(f, character, &x0, &y0, &x1, &y1);
    // I think stbtt uses math notation for coordinate and measurements.
    // TextMetrics uses absolute distances I think, so descent is measured in positive units down
    result.width = advanceWidth * scale;
    result.fontBoundingBoxAscent = ascent * scale;
    result.fontBoundingBoxDescent = -descent * scale;
    return result;
}
Canvas &Renderer::get_canvas() {
    return canvas;
}
