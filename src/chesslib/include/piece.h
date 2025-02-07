#if !defined(CHESSLIB_PIECE_H)
#define CHESSLIB_PIECE_H

class Piece {
    public:
        static constexpr int None = 0;
        static constexpr int King = 1;
        static constexpr int Pawn = 2;
        static constexpr int Knight = 3;
        static constexpr int Bishop = 5; // Sliding pieces: bit 3 is set
        static constexpr int Rook = 6;
        static constexpr int Queen = 7;

        static constexpr int White = 8;
        static constexpr int Black = 16;

        static bool isColor(int piece, int color);
        static int getColor(int piece);
        static int getPieceType(int piece);
        static bool isRookOrQueen(int piece);
        static bool isBishopOrQueen(int piece);
        static bool isSlidingPiece(int piece);
};

#endif
