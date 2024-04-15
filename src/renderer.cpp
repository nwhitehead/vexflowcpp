#include "renderer.h"

#include <iostream>
#include <stdexcept>

extern unsigned char external_Bravura_otf[];

Renderer::Renderer() : canvas(800, 600) {
    int success = stbtt_InitFont(&font, external_Bravura_otf, stbtt_GetFontOffsetForIndex(external_Bravura_otf, 0));
    if (!success) {
        throw std::runtime_error("Bravura font could not be initialized");
    }
    size = 150.0;
    scale = stbtt_ScaleForPixelHeight(&font, size);
    std::cout << "SCALE=" << scale << std::endl;
}
Renderer::~Renderer() {
}
void Renderer::draw_character(int x, int y, int character) {
    int w, h, xoff, yoff;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, character, &w, &h, &xoff, &yoff);
    std::cout << "Renderer:draw_character " << character << " x=" << x << " y=" << y << " w=" << w << " h=" << h << " xoff=" << xoff << " yoff=" << yoff << std::endl;
    canvas.blit(x + xoff, y + yoff, bitmap, w, h);
    std::free(bitmap);
}
CodepointMetrics Renderer::measure_character(int character) {
    CodepointMetrics result;
    int advanceWidth, leftSideBearing;
    stbtt_GetCodepointHMetrics(&font, character, &advanceWidth, &leftSideBearing);
    result.width = (advanceWidth - leftSideBearing) * scale;
    return result;
}
Canvas &Renderer::get_canvas() {
    return canvas;
}
