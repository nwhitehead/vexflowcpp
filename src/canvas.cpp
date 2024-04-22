#include "canvas.h"

#include <algorithm>
#include <iostream>

#include "stb_image_write.h"

Canvas::Canvas(int width_p, int height_p) : width(width_p), height(height_p), data(width * height) {
}

Canvas::~Canvas() {
}

void Canvas::resize(int width_p, int height_p) {
    width = width_p;
    height = height_p;
    data = std::vector<uint8_t>(width * height);
}

void Canvas::blit(int x, int y, uint8_t *src, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int value = src[j + i * w];
            blend(x + j, y + i, value);
        }
    }
}

void Canvas::fill_rect(double x, double y, double w, double h) {
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

void Canvas::draw_line(double x0, double y0, double x1, double y1) {
    //std::cout << "Canvas::draw_line" << std::endl;
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

void Canvas::debug() {
    uint64_t sum{0};
    for (int offset = 0; offset < width * height; offset++) {
        sum += data[offset];
    }
    std::cout << "sum=" << sum << std::endl;
}

uint32_t pack(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (r << 0) | (g << 8) | (b << 16) | (a << 24);
}
void Canvas::save(std::string filename) {
    // Convert meaning of (0, 255) from (transparent, opaque) TO (white, black)
    std::vector<uint32_t> output(data.size());
    std::transform(data.begin(), data.end(), output.begin(), [](uint8_t value) -> uint32_t {
        return pack(255 - value, 255 - value, 255 - value, 255);
    });
    stbi_write_png(filename.c_str(), width, height, 4, output.data(), width * 4);
}
