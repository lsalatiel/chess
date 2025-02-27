#include "include/board_evaluation.h"
#include "include/board.h"
#include "include/piece.h"

BoardEvaluation::BoardEvaluation() {
    whiteMaterial = 0;
    blackMaterial = 0;
}

void BoardEvaluation::evaluate(int *squares) {
    whiteMaterial = 0;
    blackMaterial = 0;

    for (int i = 0; i < 64; i++) {
        if (squares[i] == Piece::None)
            continue;

        int piece = squares[i];
        int pieceType = Piece::get_piece_type(piece);
        int pieceColor = Piece::get_piece_color(piece);

        int pieceValue = 0;
        switch (pieceType) {
            case Piece::Pawn:
                pieceValue += BoardEvaluation::PAWN;
                break;
            case Piece::Knight:
                pieceValue += BoardEvaluation::KNIGHT;
                break;
            case Piece::Bishop:
                pieceValue += BoardEvaluation::BISHOP;
                break;
            case Piece::Rook:
                pieceValue += BoardEvaluation::ROOK;
                break;
            case Piece::Queen:
                pieceValue += BoardEvaluation::QUEEN;
                break;
        }

        if (pieceColor == Piece::White)
            whiteMaterial += pieceValue;
        else
            blackMaterial += pieceValue;
    }

    whiteAdvantage = whiteMaterial - blackMaterial;
    blackAdvantage = blackMaterial - whiteMaterial;
}
