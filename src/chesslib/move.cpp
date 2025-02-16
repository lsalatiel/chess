#include "include/move.h"
#include <iostream>

int Move::squaresToEdge[64][8];
std::unordered_map<int, std::list<int>> Move::possibleMoves;

void Move::calculate_squares_to_edge() {
    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            int square = rank * 8 + file;

            int numNorth = rank;
            int numSouth = 7 - rank;
            int numEast = 7 - file;
            int numWest = file;

            int numNorthEast = std::min(numNorth, numEast);
            int numNorthWest = std::min(numNorth, numWest);
            int numSouthEast = std::min(numSouth, numEast);
            int numSouthWest = std::min(numSouth, numWest);

            squaresToEdge[square][0] = numNorth;
            squaresToEdge[square][1] = numSouth;
            squaresToEdge[square][2] = numEast;
            squaresToEdge[square][3] = numWest;
            squaresToEdge[square][4] = numNorthEast;
            squaresToEdge[square][5] = numNorthWest;
            squaresToEdge[square][6] = numSouthEast;
            squaresToEdge[square][7] = numSouthWest;
        }
    }
}

std::list<int> Move::get_moves_for_square(int fromSquare) {
    return Move::possibleMoves[fromSquare];
}

void Move::generate_moves(Board &board) {
    Move::possibleMoves.clear();

    for (int fromSquare = 0; fromSquare < 64; fromSquare++) {
        int piece = board.squares[fromSquare];
        int pieceColor = Piece::get_piece_color(piece);
        if (piece == Piece::None || pieceColor != board.colorToMove) {
            continue;
        }

        int pieceType = Piece::get_piece_type(piece);
        if (pieceType == Piece::Pawn)
            Move::generate_pawn_moves(board, piece, fromSquare, Move::possibleMoves[fromSquare]);
        else if (pieceType == Piece::King)
            Move::generate_king_moves(board, piece, fromSquare, Move::possibleMoves[fromSquare]);
    }
}

void Move::generate_pawn_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {
    int rank = fromSquare / 8;
    bool pawnTwoForward = false;
    int pieceColor = Piece::get_piece_color(piece);
    if ((pieceColor == Piece::White && rank == 6) ||
        (pieceColor == Piece::Black && rank == 1)) {
        pawnTwoForward = true;
    }

    int movingDirectionIndex = pieceColor == Piece::White ? 0 : 1;
    int capturingDirectionIndexArray[2] = { pieceColor == Piece::White ? 4 : 7, pieceColor == Piece::White ? 5 : 6 };
    
    if (Move::squaresToEdge[fromSquare][movingDirectionIndex] > 0) {
        int toSquare = fromSquare + Move::directionsOffsets[movingDirectionIndex];
        if (board.squares[toSquare] == Piece::None) {
            moves.push_back(toSquare);
            if (pawnTwoForward) {
                toSquare += Move::directionsOffsets[movingDirectionIndex];
                if (board.squares[toSquare] == Piece::None) {
                    moves.push_back(toSquare);
                }
            }
        }
    }

    // missing en passant
    for (int directionIndex : capturingDirectionIndexArray) {
        if (Move::squaresToEdge[fromSquare][directionIndex] == 0)
            continue;

        int toSquare = fromSquare + Move::directionsOffsets[directionIndex];
        if (board.squares[toSquare] != Piece::None &&
            Piece::get_piece_color(board.squares[toSquare]) != pieceColor) {
            moves.push_back(toSquare);
        }
    }
}

void Move::generate_knight_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {

}

void Move::generate_king_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {
    for (int i = 0; i < 8; i++) {
        if (Move::squaresToEdge[fromSquare][i] == 0)
            continue;

        int toSquare = fromSquare + Move::directionsOffsets[i];
        if (board.squares[toSquare] == Piece::None ||
            Piece::get_piece_color(board.squares[toSquare]) != Piece::get_piece_color(piece))
            moves.push_back(fromSquare + Move::directionsOffsets[i]);
    }
}

void Move::generate_sliding_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {

}
