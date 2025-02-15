#if !defined(CHESSLIB_BOARD_H)
#define CHESSLIB_BOARD_H

#include <iostream>

class Board {
    public:
        int squares[64];
        int colorToMove;

        Board();
        Board(std::string fen);
        void reset();
        bool make_move(int fromSquare, int toSquare, int piece);
};

#endif
