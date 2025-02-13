#if !defined(CHESSLIB_MOVE_H)
#define CHESSLIB_MOVE_H

#include "piece.h"
#include <vector>

class Move {
    public:
        int fromSquare;
        int toSquare;
        static int squaresToEdge[64][8];
        static constexpr int directionsOffsets[8] = { -1, 1, -8, 8, -9, 9, -7, 7 };

        static void calculate_squares_to_edge();

        // GENERATE POSSIBLE EVERY POSSIBLE MOVE FOR EACH PIECE IN A TURN! (just the turns color)
        // List<Move> generate_moves(Board &board, int square);
};

#endif
