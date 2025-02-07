#include "include/piece.h"

// Masks
constexpr int typeMask = 0b00111;
constexpr int blackMask = 0b10000;
constexpr int whiteMask = 0b01000;
constexpr int colourMask = whiteMask | blackMask;

bool Piece::isColor(int piece, int colour) {
    return (piece & colourMask) == colour;
}

int Piece::getColor(int piece) {
    return piece & colourMask;
}

int Piece::getPieceType(int piece) {
    return piece & typeMask;
}

bool Piece::isRookOrQueen(int piece) {
    return (piece & 0b110) == 0b110;
}

bool Piece::isBishopOrQueen(int piece) {
    return (piece & 0b101) == 0b101;
}

bool Piece::isSlidingPiece(int piece) {
    return (piece & 0b100) != 0;
}
