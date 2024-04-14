
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "javascript_runtime.h"
#include "stb_image_write.h"
#include "stb_truetype.h"

extern unsigned char external_Bravura_otf[];

std::string read_file(std::string filename) {
    // I hate C++ with a passion
    std::ostringstream buf;
    std::ifstream input{filename};
    buf << input.rdbuf();
    return buf.str();
}

unsigned char ttf_buffer[1<<25];

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
        size = 43.0f;
        scale_height = stbtt_ScaleForPixelHeight(&font, size);
    }
};

int main(int argc, char *argv[]) {
    std::cout << "Offline C++ VexFlow render" << std::endl;

    Canvas canvas{800, 600};
    std::string font_data = read_file("../external/Bravura.otf");
    stbtt_fontinfo font;
    unsigned char *font_data_p = reinterpret_cast<unsigned char *>(font_data.data());
    int success = stbtt_InitFont(&font, font_data_p, stbtt_GetFontOffsetForIndex(font_data_p, 0));
    if (!success) {
        std::cerr << "stbtt_InitFont returned failure" << std::endl;
        abort();
    }

    float scale_height = stbtt_ScaleForPixelHeight(&font, 43);

    std::cout << "scale_height=" << scale_height << std::endl;
    int c = 0xE050; // G clef
    int w, h;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, 0, scale_height, c, &w, &h, nullptr, nullptr);

    stbi_write_png("out.png", w, h, 1, bitmap, w);

    return EXIT_SUCCESS;
}
