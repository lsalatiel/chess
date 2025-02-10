#include "include/board.h"
#include "include/piece.h"

void loadPositionFromFen(std::string fen, Board &board);

Board::Board() {
    reset();
    loadPositionFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", *this);
    whiteToMove = true;
}

Board::Board(std::string fen) {
    reset();
    loadPositionFromFen(fen, *this);
    whiteToMove = true;
}

void Board::reset() {
    for (int i = 0; i < 64; i++) {
        squares[i] = Piece::None;
    }
}

void Board::make_move(int fromSquare, int toSquare, int piece) {
    if (toSquare < 0 || toSquare >= 64) {
        return;
    }
    int pieceColor = Piece::get_piece_color(piece);
    if ((whiteToMove && pieceColor != Piece::White) ||
        (!whiteToMove && pieceColor != Piece::Black)) {
        return;
    }
    // basic move without rules
    squares[toSquare] = piece;
    squares[fromSquare] = Piece::None;
    whiteToMove = !whiteToMove;
    // PAWN
    if (Piece::get_piece_type(piece) == Piece::Pawn) {

    }
}

void loadPositionFromFen(std::string fen, Board &board) {
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
