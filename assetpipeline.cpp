#include "PPU466.hpp"
#include "load_save_png.hpp"
#include "read_write_chunk.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>

// Helper function to find a color index in the palette
int find_color_index(glm::u8vec4 color, const std::vector<glm::u8vec4>& palette) {
    for (size_t i = 0; i < palette.size(); ++i) {
        if (color == palette[i]) return i;
    }
    return -1;
}

int main() {
    glm::uvec2 size;
    std::vector<glm::u8vec4> pixel_data;

    load_png("car.png", &size, &pixel_data, LowerLeftOrigin);

    // Guard specifically against proper tile size
    if (size.x != 8 || size.y != 8) {
        std::cerr << "Error: The image must be 8x8 pixels." << std::endl;
        return 1;
    }

    // Collect up to 4 unique colors (assume max possible is 4 in png)
    std::vector<glm::u8vec4> unique_colors;
    for (const auto& pixel : pixel_data) {
        if (find_color_index(pixel, unique_colors) == -1) {
            if (unique_colors.size() >= 4) {
                std::cerr << "Error: More than 4 unique colors found in the image." << std::endl;
                return 1;
            }
            unique_colors.push_back(pixel);
        }
    }

    // Create the palette with 4 colors
    PPU466::Palette palette;
    for (size_t i = 0; i < unique_colors.size(); ++i) {
        palette[i] = unique_colors[i];
    }
    // Fill remaining palette slots with transparent (if any)
    for (size_t i = unique_colors.size(); i < 4; ++i) {
        palette[i] = glm::u8vec4(0, 0, 0, 0);
    }

    // Generate the tile data
    PPU466::Tile tile;
    for (uint32_t y = 0; y < 8; ++y) {
        uint8_t bit0 = 0;
        uint8_t bit1 = 0;
        for (uint32_t x = 0; x < 8; ++x) {
            glm::u8vec4 color = pixel_data[y * 8 + x];
            int color_index = find_color_index(color, unique_colors);
            if (color_index == -1) {
                std::cerr << "Error: Could not find color in the palette." << std::endl;
                return 1;
            }
            // Map color index to bits:
            if (color_index & 1) bit0 |= (1 << x);
            if (color_index & 2) bit1 |= (1 << x);
        }
        tile.bit0[y] = bit0;
        tile.bit1[y] = bit1;
    }

    // Write the data to a binary file, from https://15466.courses.cs.cmu.edu/lesson/assets
    std::ofstream output("car_sprite.bin", std::ios::binary);
    if (!output) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }

    // Write the palette
    std::vector<PPU466::Palette> palettes = { palette };
    write_chunk("PALT", palettes, &output);

    std::ofstream output2("car_sprite_tile.bin", std::ios::binary);
    if (!output) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }

    // Write the tile
    std::vector<PPU466::Tile> tiles = { tile };
    write_chunk("TILE", tiles, &output2);

    output.close();

    std::cout << "Sprite and palette successfully written to car_sprite.bin." << std::endl;

    return 0;
}
