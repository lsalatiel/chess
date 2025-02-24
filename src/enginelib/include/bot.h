#if !defined(ENGINELIB_BOT_H)
#define ENGINELIB_BOT_H

#include <unordered_map>
#include <list>
#include <tuple>
#include "../../chesslib/include/board.h"

class Bot {
    public:
        int pieceColor;
        Bot();
        Bot(int pieceColor);
        void make_move(Board &board, std::unordered_map<int, std::list<int>> possibleMoves);
    private:
        std::tuple<int, int> pick_random_move(Board &board, std::unordered_map<int, std::list<int>>);
};

#endif
