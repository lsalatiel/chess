#include "include/bot.h"
#include "../chesslib/include/move.h"
#include <ctime>
#include <cstdlib>

Bot::Bot() {
    pieceColor = Piece::Black;
}

Bot::Bot(int _pieceColor) {
    pieceColor = _pieceColor;
}

void Bot::make_move(Board &board, std::unordered_map<int, std::list<int>> possibleMoves) {
    std::tuple<int, int> move = pick_random_move(board, possibleMoves);
    int fromSquare = std::get<0>(move);
    int toSquare = std::get<1>(move);

    board.make_move(fromSquare, toSquare, board.squares[fromSquare], possibleMoves[fromSquare], false);
}

std::tuple<int, int> Bot::pick_random_move(Board &board, std::unordered_map<int, std::list<int>> possibleMoves) {
    srand(time(0));
    std::list<int> possibleFromSquares;
    for (int i = 0; i < 64; i++) {
        if (possibleMoves[i].size() > 0) {
            possibleFromSquares.push_back(i);
        }
    }

    int fromSquareIndex = rand() % possibleFromSquares.size();
    std::list<int>::iterator itFromSquare = possibleFromSquares.begin();
    std::advance(itFromSquare, fromSquareIndex);
    int fromSquare = *itFromSquare;

    int toSquareIndex = rand() % possibleMoves[fromSquare].size();
    std::list<int>::iterator itToSquare = possibleMoves[fromSquare].begin();
    std::advance(itToSquare, toSquareIndex);
    int toSquare = *itToSquare;

    return std::tuple<int, int>({fromSquare, toSquare});
}
