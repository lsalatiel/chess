#include "include/piece.h"

// Masks
constexpr int typeMask = 0b00111;
constexpr int blackMask = 0b10000;
constexpr int whiteMask = 0b01000;
constexpr int colorMask = whiteMask | blackMask;

bool Piece::is_color(int piece, int colour) {
    return (piece & colorMask) == colour;
}

int Piece::get_piece_color(int piece) {
    return piece & colorMask;
}

int Piece::get_piece_type(int piece) {
    return piece & typeMask;
}

bool Piece::is_rook_or_queen(int piece) {
    return (piece & 0b110) == 0b110;
}

bool Piece::is_bishop_or_queen(int piece) {
    return (piece & 0b101) == 0b101;
}

bool Piece::is_sliding_piece(int piece) {
    return (piece & 0b100) != 0;
}
