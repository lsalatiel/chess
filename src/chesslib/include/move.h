#include <list>
#if !defined(CHESSLIB_MOVE_H)
#define CHESSLIB_MOVE_H

#include "piece.h"
#include "board.h"
#include <unordered_map>
#include <list>

class Move {
    public:
        static int squaresToEdge[64][8];
        static constexpr int directionsOffsets[8] = { -8, 8, 1, -1, -7, -9, 9, 7 }; // -8: N, 8: S, 1: E, -1: W
        static std::unordered_map<int, std::list<int>> possibleMoves; // key: fromSquare, value: list of toSquares

        static constexpr int whiteIndex = 0;
        static constexpr int blackIndex = 1;
        static bool kingHasMoved[2];
        static bool rookShortCastleHasMoved[2];
        static bool rookLongCastleHasMoved[2];
        static bool enPasssant;
        static int moveCount;

        static void calculate_squares_to_edge();
        static void generate_moves(Board &board, std::unordered_map<int, std::list<int>> &possibleMoves, bool handleCheck);
        static std::list<int> get_moves_for_square(int fromSquare);

    private:
        static void generate_pawn_moves(Board &board, int piece, int fromSquare, std::list<int> &moves, bool checkEnPassant);
        static void generate_knight_moves(Board &board, int piece, int fromSquare, std::list<int> &moves);
        static void generate_king_moves(Board &board, int piece, int fromSquare, std::list<int> &moves, bool handleCheck);
        static void generate_sliding_moves(Board &board, int piece, int fromSquare, std::list<int> &moves);
        static void remove_illegal_moves(Board &board, std::unordered_map<int, std::list<int>> &possibleMoves);
};

#endif
