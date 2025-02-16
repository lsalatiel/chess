#include <list>
#if !defined(CHESSLIB_MOVE_H)
#define CHESSLIB_MOVE_H

#include "piece.h"
#include "board.h"
#include <unordered_map>
#include <list>

class Move {
    public:
        /* int fromSquare; */
        /* int toSquare; */
        static int squaresToEdge[64][8];
        static constexpr int directionsOffsets[8] = { -8, 8, 1, -1, -7, -9, 9, 7 }; // -8: N, 8: S, 1: E, -1: W
        static std::unordered_map<int, std::list<int>> possibleMoves; // key: fromSquare, value: list of toSquares

        static void calculate_squares_to_edge();

        // GENERATE POSSIBLE EVERY POSSIBLE MOVE FOR EACH PIECE IN A TURN! (just the turns color)
        // List<Move> generate_moves(Board &board);
        static void generate_moves(Board &board);
        static std::list<int> get_moves_for_square(int fromSquare);

    private:
        static void generate_pawn_moves(Board &board, int piece, int fromSquare, std::list<int> &moves);
        static void generate_knight_moves(Board &board, int piece, int fromSquare, std::list<int> &moves);
        static void generate_king_moves(Board &board, int piece, int fromSquare, std::list<int> &moves);
        static void generate_sliding_moves(Board &board, int piece, int fromSquare, std::list<int> &moves);
};

#endif
