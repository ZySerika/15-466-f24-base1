#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <stdexcept>
#include <glm/glm.hpp>
#include "read_write_chunk.hpp"
#include <cassert>

struct Grid {
    // 0: empty, 1: green, 2: yellow, 3: red
    uint8_t pallet;
    bool is_wall;
};

struct Board {
    Grid board[28][28];
    glm::uvec2 size = glm::uvec2(28, 28); 

    // Constructor that reads from a file and fills the board
    Board(std::string const &path) {
        std::ifstream file(path);
        if (!file) {
            throw std::runtime_error("Failed to open level file.");
        }

        //board.resize(size.y, std::vector<Grid>(size.x));

        std::string line;
        for (int y = 0; y < 28; ++y) {
            std::getline(file, line);
            if (line.length() != 28) {
                throw std::runtime_error("Level file has incorrect line length.");
            }

            for (int x = 0; x < 28; ++x) {
                char c = line[x];
                if (c == '*') {
                    board[y][x].is_wall = true;
                    board[y][x].pallet = 0; // Wall, no color
                } else if (c == 'o') {
                    board[y][x].is_wall = false;
                    board[y][x].pallet = 0; // Empty pallet
                } else if (c == '.') {
                    board[y][x].is_wall = false;
                    board[y][x].pallet = 1; // Green pallet
                } else {
                    throw std::runtime_error("Invalid character in level file.");
                }
            }
        }
    }
};

constexpr uint32_t ScreenWidth = 32;
constexpr uint32_t AllWidth = 64;
constexpr uint32_t AllHeight = 60;

std::array<uint16_t, AllWidth * AllHeight> background;

// Function to create background based on the Board
void create_background(const Board& board) {
    background.fill(0);

    // Offset to center the 28x28 grid within the 32x30 grid (1 tile on each side, 2 tiles top/bottom)
    // Fitting algorithm is generated from chatGPT with prompt:
    // "I want to create a background data structure according to my level design, where I center my 28x28 grid in the first 32x30 screen-size tiles at the bottom left (in other words, I will not use scrolling for the game). 
    // Use tile index 0 for '*' as well as any tile outside the 28x28 grid, and use tile index 1 for 'o' and '.'"". 
    int x_offset = 2;
    int y_offset = 1;

    for (uint32_t y = 0; y < 28; ++y) {
        for (uint32_t x = 0; x < 28; ++x) {
            const Grid& grid = board.board[y][x];

            // Calculate the corresponding background index with the offsets
            uint32_t bg_x = x + x_offset;
            uint32_t bg_y = y + y_offset;
            uint32_t index = bg_y * ScreenWidth + bg_x;

            uint16_t tile = 0;
            if (grid.is_wall) {
                tile = 0; 
            } else {
                tile = 1; 
            }

            // Palette is determined by the pallet field in the Grid structure
            uint16_t palette_index = 0;

            // Construct the 16-bit background value
            background[index] = (palette_index << 8) | tile;
        }
    }
}

// Helper function to write binary data
template<typename T>
void write_binary(const std::string& filename, const T& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open output file.");
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(typename T::value_type));
    file.close();
}

int main() {
    Board board("maze.txt");

    create_background(board);

    std::ofstream output("board.bin", std::ios::binary);
    if (!output) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }
    std::vector<Grid> board_data = { board.board[0][0] };
    write_chunk("BRDD", board_data, &output);

    std::ofstream output2("background.bin", std::ios::binary);
    if (!output) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }
    write_chunk("BGRA", std::vector<uint16_t>(background.begin(), background.end()), &output2);

    std::cout << "Level data and background successfully written to binary files." << std::endl;
    return 0;
}
