
#pragma once

#include "canvas.h"
#include "stb_truetype.h"

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
public:
    float size;
    float scale;
    Renderer();
    ~Renderer();
    void draw_character(int x, int y, int character);
    CodepointMetrics measure_character(int character);
    Canvas &get_canvas();
};
