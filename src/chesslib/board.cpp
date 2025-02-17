#include "include/board.h"
#include "include/piece.h"
#include "include/move.h"
#include <algorithm>

const std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

void load_position_from_fen(std::string fen, Board &board);

Board::Board() {
    reset();
    load_position_from_fen(DEFAULT_FEN, *this);
    colorToMove = 8;
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

bool Board::make_move(int fromSquare, int toSquare, int piece) {
    if (toSquare < 0 || toSquare >= 64 || Piece::get_piece_color(piece) != colorToMove)
        return false;
    
    std::list<int> moves = Move::get_moves_for_square(fromSquare);

    if (std::find(moves.begin(), moves.end(), toSquare) == moves.end())
        return false;

    if (Piece::get_piece_type(piece) == Piece::Pawn && (toSquare < 8 || toSquare >= 56))
        piece = Piece::Queen | Piece::get_piece_color(piece);

    squares[toSquare] = piece;
    squares[fromSquare] = Piece::None;
    if (colorToMove == Piece::White)
        colorToMove = Piece::Black;
    else
        colorToMove = Piece::White;

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
