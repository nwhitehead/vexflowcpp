#include "canvas.h"

#include <iostream>

#include "stb_image_write.h"

Canvas::Canvas(int width_p, int height_p) : width(width_p), height(height_p), data(width * height) {
    //std::cout << "Canvas::Canvas" << std::endl;
}

Canvas::~Canvas() {
    //std::cout << "Canvas::~Canvas" << std::endl;
}

void Canvas::blit(int x, int y, uint8_t *src, int w, int h) {
    //std::cout << "Canvas::blit x=" << x << " y=" << y << std::endl;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int value = src[j + i * w];
            blend(x + j, y + i, value);
        }
    }
}

void Canvas::fill_rect(double x, double y, double w, double h) {
    //std::cout << "Canvas::fill_rect" << std::endl;
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

void Canvas::save(std::string filename) {
    //std::cout << "Canvas::save" << std::endl;
    stbi_write_png(filename.c_str(), width, height, 1, data.data(), width);
}
