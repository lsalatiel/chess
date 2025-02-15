#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include "chesslib/include/board.h"
#include "chesslib/include/piece.h"
#include "chesslib/include/move.h"

struct DragState {
    bool dragging = false;
    int selectedPiece = Piece::None;
    int fromSquare = -1;
    int mouseX = 0, mouseY = 0;
};

struct SquareState {
    bool selected = false;
    bool highlighted = false;
};

const int WINDOW_SIZE = 600;
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = WINDOW_SIZE / BOARD_SIZE;
const std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
const std::string EMPTY_FEN = "8/8/8/8/8/8/8/8";

// Colors
SDL_Color LIGHT = {240, 217, 181, 255};
SDL_Color DARK = {181, 136, 99, 255};
SDL_Color LIGHT_HIGHLIGHTED = {235, 120, 99, 255};
SDL_Color DARK_HIGHLIGHTED = {224, 104, 83, 255};

void cleanup(std::unordered_map<std::string, SDL_Texture*> &pieceTextures);
void load_piece_textures(SDL_Renderer* renderer, std::unordered_map<std::string, std::string> &pieceFiles, std::unordered_map<std::string, SDL_Texture*> &pieceTextures);
void draw_chess_board(SDL_Renderer *renderer, struct SquareState (&squareState)[64]);
void draw_chess_pieces(SDL_Renderer* renderer, int (&board)[64], std::unordered_map<std::string, SDL_Texture*> &pieceTextures, struct DragState &dragState, TTF_Font* font);
void handle_mouse_input(SDL_Event event, Board &board, struct DragState &dragState, struct SquareState (&squareState)[64]);
std::string get_piece_name(int piece);

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

    TTF_Font *font = nullptr;

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return -1;
    }

    font = TTF_OpenFont("/usr/share/fonts/TTF/SauceCodeProNerdFont-Regular.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return -1;
    }   

    // Piece file names
    std::unordered_map<std::string, std::string> pieceFiles = {
        {"wK", "imgs/wK.svg"}, {"wQ", "imgs/wQ.svg"}, {"wR", "imgs/wR.svg"},
        {"wB", "imgs/wB.svg"}, {"wN", "imgs/wN.svg"}, {"wP", "imgs/wP.svg"},
        {"bK", "imgs/bK.svg"}, {"bQ", "imgs/bQ.svg"}, {"bR", "imgs/bR.svg"},
        {"bB", "imgs/bB.svg"}, {"bN", "imgs/bN.svg"}, {"bP", "imgs/bP.svg"}
    };

    // Piece textures
    std::unordered_map<std::string, SDL_Texture*> pieceTextures;

    // Initial board setup (FEN-like)
    Board board = Board(DEFAULT_FEN);

    load_piece_textures(renderer, pieceFiles, pieceTextures);

    Move::calculate_squares_to_edge();
    Move::generate_moves(board);

    bool running = true;
    SDL_Event event;

    struct DragState dragState;
    struct SquareState squareState[64];

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if ((event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP ||
                    event.type == SDL_MOUSEMOTION)) {
                handle_mouse_input(event, board, dragState, squareState);
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        draw_chess_board(renderer, squareState);
        draw_chess_pieces(renderer, board.squares, pieceTextures, dragState, font);

        // Present the updated frame
        SDL_RenderPresent(renderer);
    }

    cleanup(pieceTextures);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Load images into textures
void load_piece_textures(SDL_Renderer* renderer, std::unordered_map<std::string, std::string> &pieceFiles, std::unordered_map<std::string, SDL_Texture*> &pieceTextures) {
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

void draw_chess_board(SDL_Renderer *renderer, struct SquareState (&squareState)[64]) {
    for (int rank = 0; rank < BOARD_SIZE; rank++) {
        for (int file = 0; file < BOARD_SIZE; file++) {
            SDL_Rect square = {file * SQUARE_SIZE, rank * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};

            // Alternate colors
            if ((rank + file) % 2 == 0) {
                if (squareState[rank * 8 + file].highlighted)
                    SDL_SetRenderDrawColor(renderer, LIGHT_HIGHLIGHTED.r, LIGHT_HIGHLIGHTED.g, LIGHT_HIGHLIGHTED.b, LIGHT_HIGHLIGHTED.a);
                else
                    SDL_SetRenderDrawColor(renderer, LIGHT.r, LIGHT.g, LIGHT.b, LIGHT.a);
            }
            else {
                if (squareState[rank * 8 + file].highlighted)
                    SDL_SetRenderDrawColor(renderer, DARK_HIGHLIGHTED.r, DARK_HIGHLIGHTED.g, DARK_HIGHLIGHTED.b, DARK_HIGHLIGHTED.a);
                else
                    SDL_SetRenderDrawColor(renderer, DARK.r, DARK.g, DARK.b, DARK.a);
            }

            SDL_RenderFillRect(renderer, &square);
        }
    }
}

// Render chess pieces
void draw_chess_pieces(SDL_Renderer* renderer, int (&board)[64], std::unordered_map<std::string, SDL_Texture*> &pieceTextures, struct DragState &dragState, TTF_Font* font) {
#ifdef DEBUG
    SDL_Color textColor = {0, 0, 0}; // Black color for text
#endif

    for (int square = 0; square < 64; square++) {
        int piece = board[square];
        int rank = square / 8;
        int file = square % 8;

        // Draw the piece if it exists
        if (piece != Piece::None) {
            if (dragState.dragging && square == dragState.fromSquare) {
                continue;
            }

            std::string pieceName = get_piece_name(piece);
            SDL_Texture* texture = pieceTextures[pieceName];
            SDL_Rect destRect = {file * SQUARE_SIZE, rank * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        }

#ifdef DEBUG
        // Render square index as text
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, std::to_string(square).c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = {file * SQUARE_SIZE, rank * SQUARE_SIZE, 20, 20}; // Position text slightly inside the square
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
#endif
    }
    // Draw dragging piece
    if (dragState.dragging && dragState.selectedPiece != Piece::None) {
        SDL_Texture* texture = pieceTextures[get_piece_name(dragState.selectedPiece)];
        SDL_Rect destRect = {dragState.mouseX - SQUARE_SIZE / 2, dragState.mouseY - SQUARE_SIZE / 2, SQUARE_SIZE, SQUARE_SIZE};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }
}

// Cleanup function
void cleanup(std::unordered_map<std::string, SDL_Texture*> &pieceTextures) {
    for (auto& texture : pieceTextures) {
        SDL_DestroyTexture(texture.second);
    }
    pieceTextures.clear();
}

void handle_mouse_input(SDL_Event event, Board &board, struct DragState &dragState, struct SquareState (&squareState)[64]) {
    int x, y;
    SDL_GetMouseState(&x, &y);

    x = std::max(0, std::min(WINDOW_SIZE - 1, x));
    y = std::max(0, std::min(WINDOW_SIZE - 1, y));

    int rank = y / SQUARE_SIZE;
    int file = x / SQUARE_SIZE;
    int square = rank * 8 + file;

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (board.squares[square] != Piece::None) {
            dragState.dragging = true;
            dragState.fromSquare = square;
            dragState.selectedPiece = board.squares[square];
            dragState.mouseX = x;
            dragState.mouseY = y;
        }
    }
    else if (event.type == SDL_MOUSEMOTION && dragState.dragging) {
        dragState.mouseX = x;
        dragState.mouseY = y;
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        if (dragState.dragging && dragState.fromSquare != square) {
            if (board.make_move(dragState.fromSquare, square, dragState.selectedPiece))
                Move::generate_moves(board);
        }
        dragState.dragging = false;
        dragState.fromSquare = -1;
        dragState.selectedPiece = Piece::None;
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
        squareState[square].highlighted = !squareState[square].highlighted;
    }
}

std::string get_piece_name(int piece) {
    std::string pieceName;

    if (Piece::is_color(piece, Piece::White)) {
        if (Piece::get_piece_type(piece) == Piece::King) {
            pieceName = "wK";
        } else if (Piece::get_piece_type(piece) == Piece::Queen) {
            pieceName = "wQ";
        } else if (Piece::get_piece_type(piece) == Piece::Rook) {
            pieceName = "wR";
        } else if (Piece::get_piece_type(piece) == Piece::Bishop) {
            pieceName = "wB";
        } else if (Piece::get_piece_type(piece) == Piece::Knight) {
            pieceName = "wN";
        } else if (Piece::get_piece_type(piece) == Piece::Pawn) {
            pieceName = "wP";
        }
    } else {
        if (Piece::get_piece_type(piece) == Piece::King) {
            pieceName = "bK";
        } else if (Piece::get_piece_type(piece) == Piece::Queen) {
            pieceName = "bQ";
        } else if (Piece::get_piece_type(piece) == Piece::Rook) {
            pieceName = "bR";
        } else if (Piece::get_piece_type(piece) == Piece::Bishop) {
            pieceName = "bB";
        } else if (Piece::get_piece_type(piece) == Piece::Knight) {
            pieceName = "bN";
        } else if (Piece::get_piece_type(piece) == Piece::Pawn) {
            pieceName = "bP";
        }
    }

    return pieceName;
}
