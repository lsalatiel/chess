#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include "guilib/include/board.h"
#include "chesslib/include/board.h"
#include "chesslib/include/piece.h"

const int WINDOW_SIZE = 600;
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = WINDOW_SIZE / BOARD_SIZE;

// Colors
SDL_Color LIGHT = {240, 217, 181, 255};
SDL_Color DARK = {181, 136, 99, 255};

void cleanup();
void load_piece_textures(SDL_Renderer* renderer);
void draw_chess_board(SDL_Renderer *renderer);
void draw_chess_pieces(SDL_Renderer *renderer);

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    load_piece_textures(renderer);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        draw_chess_board(renderer);
        draw_chess_pieces(renderer);

        // Present the updated frame
        SDL_RenderPresent(renderer);
    }

    cleanup();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Piece file names
std::unordered_map<std::string, std::string> pieceFiles = {
    {"wK", "imgs/wK.svg"}, {"wQ", "imgs/wQ.svg"}, {"wR", "imgs/wR.svg"},
    {"wB", "imgs/wB.svg"}, {"wN", "imgs/wN.svg"}, {"wP", "imgs/wP.svg"},
    {"bK", "imgs/bK.svg"}, {"bQ", "imgs/bQ.svg"}, {"bR", "imgs/bR.svg"},
    {"bB", "imgs/bB.svg"}, {"bN", "imgs/bN.svg"}, {"bP", "imgs/bP.svg"}
};

// Initial board setup (FEN-like)
std::string board[8][8] = {
    {"bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"},
    {"bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"},
    {".", ".", ".", ".", ".", ".", ".", "."},
    {".", ".", ".", ".", ".", ".", ".", "."},
    {".", ".", ".", ".", ".", ".", ".", "."},
    {".", ".", ".", ".", ".", ".", ".", "."},
    {"wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"},
    {"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}
};

// Piece textures
std::unordered_map<std::string, SDL_Texture*> pieceTextures;

// Load images into textures
void load_piece_textures(SDL_Renderer* renderer) {
    for (auto& piece : pieceFiles) {
        SDL_Surface* surface = IMG_Load(piece.second.c_str());
        if (!surface) {
            std::cerr << "Failed to load image " << piece.second << ": " << IMG_GetError() << std::endl;
            continue;
        }
        pieceTextures[piece.first] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void draw_chess_board(SDL_Renderer *renderer) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};

            // Alternate colors
            if ((row + col) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, LIGHT.r, LIGHT.g, LIGHT.b, LIGHT.a);
            } else {
                SDL_SetRenderDrawColor(renderer, DARK.r, DARK.g, DARK.b, DARK.a);
            }

            SDL_RenderFillRect(renderer, &square);
        }
    }
}

// Render chess pieces
void draw_chess_pieces(SDL_Renderer* renderer) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            std::string piece = board[row][col];
            if (piece != ".") {
                SDL_Texture* texture = pieceTextures[piece];
                SDL_Rect destRect = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                SDL_RenderCopy(renderer, texture, nullptr, &destRect);
            }
        }
    }
}

// Cleanup function
void cleanup() {
    for (auto& texture : pieceTextures) {
        SDL_DestroyTexture(texture.second);
    }
    pieceTextures.clear();
}
