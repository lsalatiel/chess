#include "include/board.h"
#include "include/piece.h"
#include "include/move.h"
#include <algorithm>

const std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

Board::Board() {
    reset();
    load_position_from_fen(DEFAULT_FEN, *this);
}

Board::Board(std::string fen) {
    reset();
    load_position_from_fen(fen, *this);
}

void Board::reset() {
    for (int i = 0; i < 64; i++) {
        squares[i] = Piece::None;
        squareState[i].played = false;
        squareState[i].highlighted = false;
    }

    colorToMove = Piece::White;
    gameEnded = false;
    colorWinner = -1;
    stalemate = false;
    drawByRepetition = false;
    insufficientMaterial = false;
    boardEvaluation.whiteMaterial = 0;
    boardEvaluation.blackMaterial = 0;
}

void Board::evaluate_board() {
    boardEvaluation.evaluate(this->squares);
}

// 0: invalid move, 1: valid move, 2: white wins, 3: black wins, 4: draw
int Board::make_move(int fromSquare, int toSquare, int piece, std::list<int> &moves, bool simulate) {
    if (toSquare < 0 || toSquare >= 64 || Piece::get_piece_color(piece) != colorToMove)
        return false;
    int pieceColor = colorToMove;

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

        if (!simulate) {
            if (pieceColor == Piece::White)
                Move::kingHasMoved[Move::whiteIndex] = true;
            else
                Move::kingHasMoved[Move::blackIndex] = true;
        }
    }

    if (Piece::get_piece_type(piece) == Piece::Rook && !simulate) {
        if (fromSquare == 0)
            Move::rookLongCastleHasMoved[Move::blackIndex] = true;
        else if (fromSquare == 7)
            Move::rookShortCastleHasMoved[Move::blackIndex] = true;
        else if (fromSquare == 56)
            Move::rookLongCastleHasMoved[Move::whiteIndex] = true;
        else if (fromSquare == 63)
            Move::rookShortCastleHasMoved[Move::whiteIndex] = true;
    }

    if (!simulate) {
        if (squares[toSquare] == Piece::None)
            Move::moveCount++;
        else
            Move::moveCount = 0;
    }

    if (Move::enPasssant) {
        int diff = colorToMove == Piece::White ? 8 : -8;
        squares[toSquare + diff] = Piece::None;
        if (!simulate)
            Move::enPasssant = false;
    }

    squares[toSquare] = piece;
    squares[fromSquare] = Piece::None;
    
    if (colorToMove == Piece::White)
        colorToMove = Piece::Black;
    else
        colorToMove = Piece::White;

    squareState[fromSquare].played = true;
    squareState[toSquare].played = true;

    for (int i = 0; i < 64; i++) {
        if (squareState[i].played && i != toSquare && i != fromSquare)
            squareState[i].played = false;
    }

    /* if (Move::moveCount == 50) */
    /*     return 4; */

    if (!simulate)
        Move::generate_moves(*this, Move::possibleMoves, true);

    return 1;
}

void Board::load_position_from_fen(std::string fen, Board &board) {
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
