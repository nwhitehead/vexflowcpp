
#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

class Canvas {
private:
    // Dimensions of canvas in pixels
    int width;
    int height;
    // Canvas bitmap data, 0 to 255 per pixel.
    // Values are intensity of what we have drawn, 0 is nothing (transparent to paper), 255 is pure color (black if black on white paper)
    std::vector<uint8_t> data;
    void debug();

public:
    Canvas(int width_p, int height_p);
    ~Canvas();
    void resize(int width_p, int height_p);
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
    void blit(int x, int y, uint8_t *src, int w, int h);
    void fill_rect(double x, double y, double w, double h);
    void draw_line(double x0, double y0, double x1, double y1);
    void save(std::string filename);
};
