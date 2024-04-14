
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "javascript_runtime.h"
#include "stb_image_write.h"
#include "stb_truetype.h"

std::string read_file(std::string filename) {
    // I hate C++ with a passion
    std::ostringstream buf;
    std::ifstream input{filename};
    buf << input.rdbuf();
    return buf.str();
}

unsigned char ttf_buffer[1<<25];

int main(int argc, char *argv[]) {
    std::cout << "Offline C++ VexFlow render" << std::endl;

    std::string font_data = read_file("../external/Bravura.otf");
    stbtt_fontinfo font;
    unsigned char *font_data_p = reinterpret_cast<unsigned char *>(font_data.data());
    int success = stbtt_InitFont(&font, font_data_p, stbtt_GetFontOffsetForIndex(font_data_p, 0));
    if (!success) {
        std::cerr << "stbtt_InitFont returned failure" << std::endl;
        abort();
    }

    float scale_height = stbtt_ScaleForPixelHeight(&font, 43 * 20);

    std::cout << "scale_height=" << scale_height << std::endl;
    int c = 0xE050;
    int w, h;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, 0, scale_height, c, &w, &h, nullptr, nullptr);

    stbi_write_png("out.png", w, h, 1, bitmap, w);

    return EXIT_SUCCESS;
}
