#if !defined(CHESSLIB_BOARD_H)
#define CHESSLIB_BOARD_H

#include <iostream>

class Board {
    public:
        int square[64];

        Board();
        Board(std::string fen);
        void reset();
};

#endif
