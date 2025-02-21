#if !defined(CHESSLIB_BOARD_H)
#define CHESSLIB_BOARD_H

#include <iostream>
#include <list>

struct SquareState {
    /* bool selected = false; */
    bool highlighted = false;
    bool played = false;
};

class Board {
    public:
        int squares[64];
        int colorToMove;
        struct SquareState squareState[64];

        Board();
        Board(std::string fen);
        void reset();
        // 0: invalid move, 1: valid move, 2: white wins, 3: black wins, 4: draw
        int make_move(int fromSquare, int toSquare, int piece, std::list<int> &moves, bool simulate);
};

#endif
