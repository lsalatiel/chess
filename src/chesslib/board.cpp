#include "include/board.h"
#include "include/piece.h"
#include "include/move.h"
#include <algorithm>

const std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

void load_position_from_fen(std::string fen, Board &board);

Board::Board() {
    reset();
    load_position_from_fen(DEFAULT_FEN, *this);
    colorToMove = Piece::White;
    for (int i = 0; i < 2; i++) {
        Move::kingHasMoved[i] = false;
        Move::rookShortCastleHasMoved[i] = false;
        Move::rookLongCastleHasMoved[i] = false;
    }
    Move::moveCount = 0;
}

Board::Board(std::string fen) {
    reset();
    load_position_from_fen(fen, *this);
    colorToMove = 8;
}

void Board::reset() {
    for (int i = 0; i < 64; i++) {
        squares[i] = Piece::None;
    }
}

// 0: invalid move, 1: valid move, 2: white wins, 3: black wins, 4: draw
int Board::make_move(int fromSquare, int toSquare, int piece) {
    if (toSquare < 0 || toSquare >= 64 || Piece::get_piece_color(piece) != colorToMove)
        return false;
    int pieceColor = colorToMove;
    
    std::list<int> moves = Move::get_moves_for_square(fromSquare);

    if (std::find(moves.begin(), moves.end(), toSquare) == moves.end())
        return false;

    // handling promotion
    if (Piece::get_piece_type(piece) == Piece::Pawn && (toSquare < 8 || toSquare >= 56))
        piece = Piece::Queen | pieceColor;

    // handling castle
    if (Piece::get_piece_type(piece) == Piece::King) {
        if (toSquare - fromSquare == 2) {
            squares[toSquare - 1] = squares[toSquare + 1];
            squares[toSquare + 1] = Piece::None;
        }
        else if (fromSquare - toSquare == 2) {
            squares[toSquare + 1] = squares[toSquare - 2];
            squares[toSquare - 2] = Piece::None;
        }

        if (pieceColor == Piece::White)
            Move::kingHasMoved[Move::whiteIndex] = true;
        else
            Move::kingHasMoved[Move::blackIndex] = true;
    }
    if (Piece::get_piece_type(piece) == Piece::Rook) {
        if (fromSquare == 0)
            Move::rookLongCastleHasMoved[Move::blackIndex] = true;
        else if (fromSquare == 7)
            Move::rookShortCastleHasMoved[Move::blackIndex] = true;
        else if (fromSquare == 56)
            Move::rookLongCastleHasMoved[Move::whiteIndex] = true;
        else if (fromSquare == 63)
            Move::rookShortCastleHasMoved[Move::whiteIndex] = true;
    }

    if (squares[fromSquare] != Piece::None)
        Move::moveCount++;
    else
        Move::moveCount = 0;

    squares[toSquare] = piece;
    squares[fromSquare] = Piece::None;
    if (colorToMove == Piece::White)
        colorToMove = Piece::Black;
    else
        colorToMove = Piece::White;

    if (Move::moveCount == 50)
        return 4;

    return true;
}

void load_position_from_fen(std::string fen, Board &board) {
    // starting on top left
    int file = 0;
    int rank = 0;
    for (char c : fen) {
        if (c == '/') { continue; }
        if (isdigit(c)) {
            file += c - '0';
        } else {
            int pieceType = Piece::None;
            int pieceColor = isupper(c) ? Piece::White : Piece::Black; 
            c = tolower(c);
            switch (c) {
                case 'r': pieceType = Piece::Rook; break;
                case 'n': pieceType = Piece::Knight; break;
                case 'b': pieceType = Piece::Bishop; break;
                case 'q': pieceType = Piece::Queen; break;
                case 'k': pieceType = Piece::King; break;
                case 'p': pieceType = Piece::Pawn; break;
            }
            board.squares[rank * 8 + file] = pieceType | pieceColor;
            file++;
        }
    }
}
