#include "include/move.h"
#include <iostream>

int Move::squaresToEdge[64][8];

void Move::calculate_squares_to_edge() {
    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            int square = rank * 8 + file;

            int numNorth = rank;
            int numSouth = 7 - rank;
            int numEast = 7 - file;
            int numWest = file;

            int numNorthEast = std::min(numNorth, numEast);
            int numNorthWest = std::min(numNorth, numWest);
            int numSouthEast = std::min(numSouth, numEast);
            int numSouthWest = std::min(numSouth, numWest);

            squaresToEdge[square][0] = numNorth;
            squaresToEdge[square][1] = numSouth;
            squaresToEdge[square][2] = numEast;
            squaresToEdge[square][3] = numWest;
            squaresToEdge[square][4] = numNorthEast;
            squaresToEdge[square][5] = numNorthWest;
            squaresToEdge[square][6] = numSouthEast;
            squaresToEdge[square][7] = numSouthWest;
        }
    }
}
