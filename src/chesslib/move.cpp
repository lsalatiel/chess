#include "include/move.h"
#include <iostream>

int Move::squaresToEdge[64][8];
std::unordered_map<int, std::list<int>> Move::possibleMoves;
bool Move::kingHasMoved[2] = {false, false};
bool Move::rookShortCastleHasMoved[2] = {false, false};
bool Move::rookLongCastleHasMoved[2] = {false, false};
bool Move::enPasssant = false;
int Move::moveCount = 0;

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
        else if (pieceType == Piece::Knight)
            Move::generate_knight_moves(board, piece, fromSquare, Move::possibleMoves[fromSquare]);
        else if (Piece::is_sliding_piece(piece))
            Move::generate_sliding_moves(board, piece, fromSquare, Move::possibleMoves[fromSquare]);
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
        int diff = board.colorToMove == Piece::White ? 8 : -8;
        if (board.squares[toSquare] != Piece::None &&
            Piece::get_piece_color(board.squares[toSquare]) != pieceColor) {
            moves.push_back(toSquare);
        }
        else if (Piece::get_piece_type(board.squares[toSquare + diff]) == Piece::Pawn &&
                board.squareState[toSquare + 8].played && board.squareState[toSquare - 8].played) {
            moves.push_back(toSquare);
            Move::enPasssant = true;
        }
    }
}

void Move::generate_knight_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {
    for (int i = 0; i < 4; i++) {
        if (Move::squaresToEdge[fromSquare][i] < 2)
            continue;
        int auxSquare = fromSquare + 2 * Move::directionsOffsets[i];
        int directionsIndex[2];
        if (i == 0 || i == 1) { directionsIndex[0] = 2; directionsIndex[1] = 3; }
        else { directionsIndex[0] = 0; directionsIndex[1] = 1; }
        
        for (int j = 0; j < 2; j++) {
            if (Move::squaresToEdge[auxSquare][directionsIndex[j]] == 0)
                continue;
            int toSquare = auxSquare + Move::directionsOffsets[directionsIndex[j]];
            if (board.squares[toSquare] == Piece::None ||
                Piece::get_piece_color(board.squares[toSquare]) != Piece::get_piece_color(piece))
                moves.push_back(toSquare);
        }
    }
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
    
    // castling
    int colorIndex = Piece::get_piece_color(piece) == Piece::White ? Move::whiteIndex : Move::blackIndex;
    if (!Move::kingHasMoved[colorIndex]) {
        if (!Move::rookShortCastleHasMoved[colorIndex]) {
            bool canCastle = true;
            for (int i = 1; i < 3; i++) {
                if (board.squares[fromSquare + i] != Piece::None)
                    canCastle = false;
            }
            if (canCastle) {
                moves.push_back(fromSquare + 2);
            }
        }
        if (!Move::rookLongCastleHasMoved[colorIndex]) {
            bool canCastle = true;
            for (int i = 1; i < 4; i++) {
                if (board.squares[fromSquare - i] != Piece::None)
                    canCastle = false;
            }
            if (canCastle) {
                moves.push_back(fromSquare - 2);
            }
        }
    }
}

void Move::generate_sliding_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {
    int startDirectionIndex = Piece::get_piece_type(piece) == Piece::Bishop ? 4 : 0;
    int endDirectionIndex = Piece::get_piece_type(piece) == Piece::Rook ? 4 : 8;

    for (int directionIndex = startDirectionIndex; directionIndex < endDirectionIndex; directionIndex++) {
        for (int i = 0; i < Move::squaresToEdge[fromSquare][directionIndex]; i++) {
            int toSquare = fromSquare + Move::directionsOffsets[directionIndex] * (i + 1);
            int pieceOnToSquare = board.squares[toSquare];
            int pieceColor = Piece::get_piece_color(piece);

            if (Piece::is_color(pieceOnToSquare, pieceColor))
                break;

            moves.push_back(toSquare);

            if (pieceOnToSquare != Piece::None && !Piece::is_color(pieceOnToSquare, pieceColor))
                break;
        }
    }
}
