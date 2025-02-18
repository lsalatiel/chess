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
        // 0: invalid move, 1: valid move, 2: white wins, 3: black wins, 4: draw
        int make_move(int fromSquare, int toSquare, int piece);
};

#endif
