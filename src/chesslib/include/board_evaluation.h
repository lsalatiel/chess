#if !defined(ENGINELIB_BOARD_EVALUATION_H)
#define ENGINELIB_BOARD_EVALUATION_H

class BoardEvaluation {
    public:
        static constexpr int PAWN = 1;
        static constexpr int KNIGHT = 3;
        static constexpr int BISHOP = 3;
        static constexpr int ROOK = 5;
        static constexpr int QUEEN = 9;
        static constexpr int STD_MATERIAL = 39;

        int whiteMaterial;
        int blackMaterial;
        int whiteAdvantage;
        int blackAdvantage;

        BoardEvaluation();
        void evaluate(int *squares);
};

#endif
