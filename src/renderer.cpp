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
    return &(fonts.find(fontname)->second).font;
}

double Renderer::get_font_scale(std::string fontname, double pixel_height) {
    return stbtt_ScaleForPixelHeight(get_font(fontname), pixel_height);
}

void Renderer::register_font(std::string filename, std::string fontname) {
    fonts.emplace(std::make_pair(fontname, Font(filename)));
}

void Renderer::draw_character(int x, int y, int character, std::string fontname, double scale) {
    int w, h, xoff, yoff;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(get_font(fontname), 0, scale, character, &w, &h, &xoff, &yoff);
    canvas.blit(x + xoff, y + yoff, bitmap, w, h);
    std::free(bitmap);
}

CodepointMetrics Renderer::measure_character(int character, std::string fontname, double scale) {
    CodepointMetrics result;
    int advanceWidth, leftSideBearing;
    stbtt_GetCodepointHMetrics(get_font(fontname), character, &advanceWidth, &leftSideBearing);
    result.width = (advanceWidth - leftSideBearing) * scale;
    return result;
}
Canvas &Renderer::get_canvas() {
    return canvas;
}
