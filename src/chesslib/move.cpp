#include "include/move.h"
#include <iostream>

int Move::squaresToEdge[64][8];
bool Move::kingHasMoved[2] = {false, false};
bool Move::rookShortCastleHasMoved[2] = {false, false};
bool Move::rookLongCastleHasMoved[2] = {false, false};
bool Move::enPasssant = false;
int Move::moveCount = 0;

int is_in_check(Board board);

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

std::list<int> Move::get_moves_for_square(int fromSquare) {
    return Move::possibleMoves[fromSquare];
}

void Move::generate_moves(Board &board, std::unordered_map<int, std::list<int>> &possibleMoves, bool handleCheck) {
    if (handleCheck)
        Move::possibleMoves.clear();

    for (int fromSquare = 0; fromSquare < 64; fromSquare++) {
        int piece = board.squares[fromSquare];
        int pieceColor = Piece::get_piece_color(piece);
        if (piece == Piece::None || pieceColor != board.colorToMove) {
            continue;
        }

        int pieceType = Piece::get_piece_type(piece);
        if (pieceType == Piece::Pawn)
            Move::generate_pawn_moves(board, piece, fromSquare, possibleMoves[fromSquare], handleCheck);
        else if (pieceType == Piece::King)
            Move::generate_king_moves(board, piece, fromSquare, possibleMoves[fromSquare], handleCheck);
        else if (pieceType == Piece::Knight)
            Move::generate_knight_moves(board, piece, fromSquare, possibleMoves[fromSquare]);
        else if (Piece::is_sliding_piece(piece))
            Move::generate_sliding_moves(board, piece, fromSquare, possibleMoves[fromSquare]);
    }
    
    if (handleCheck) {
        remove_illegal_moves(board, Move::possibleMoves);
    
        bool isTherePossibleMoves = false;
        for (int fromSquare = 0; fromSquare < 64; fromSquare++) {
            if (possibleMoves[fromSquare].size() > 0) {
                isTherePossibleMoves = true;
                break;
            }
        }

        if (!isTherePossibleMoves) {
            board.gameEnded = true;
            if (is_in_check(board))
                if (board.colorToMove == Piece::White)
                    /* board.colorWinner = Piece::White; */
                    std::cout << "White wins" << std::endl;
                else
                    /* board.colorWinner = Piece::Black; */
                    std::cout << "Black wins" << std::endl;
            else
                /* board.stalemate = true; */
                std::cout << "Stalemate" << std::endl;
        }
    }
}

void Move::generate_pawn_moves(Board &board, int piece, int fromSquare, std::list<int> &moves, bool checkEnPassant) {
    int rank = fromSquare / 8;
    bool pawnTwoForward = false;
    int pieceColor = Piece::get_piece_color(piece);
    if ((pieceColor == Piece::White && rank == 6) ||
        (pieceColor == Piece::Black && rank == 1)) {
        pawnTwoForward = true;
    }

    int movingDirectionIndex = pieceColor == Piece::White ? 0 : 1;
    int capturingDirectionIndexArray[2] = { pieceColor == Piece::White ? 4 : 7, pieceColor == Piece::White ? 5 : 6 };
    
    if (Move::squaresToEdge[fromSquare][movingDirectionIndex] > 0) {
        int toSquare = fromSquare + Move::directionsOffsets[movingDirectionIndex];
        if (board.squares[toSquare] == Piece::None) {
            moves.push_back(toSquare);
            if (pawnTwoForward) {
                toSquare += Move::directionsOffsets[movingDirectionIndex];
                if (board.squares[toSquare] == Piece::None) {
                    moves.push_back(toSquare);
                }
            }
        }
    }

    for (int directionIndex : capturingDirectionIndexArray) {
        if (Move::squaresToEdge[fromSquare][directionIndex] == 0)
            continue;

        int toSquare = fromSquare + Move::directionsOffsets[directionIndex];
        int diff = board.colorToMove == Piece::White ? 8 : -8;
        if (board.squares[toSquare] != Piece::None &&
            Piece::get_piece_color(board.squares[toSquare]) != pieceColor) {
            moves.push_back(toSquare);
        }
        else if (Piece::get_piece_type(board.squares[toSquare + diff]) == Piece::Pawn &&
                board.squareState[toSquare + 8].played && board.squareState[toSquare - 8].played) {
            moves.push_back(toSquare);
            if (checkEnPassant)
                Move::enPasssant = true;
        }
    }
}

void Move::generate_knight_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {
    for (int i = 0; i < 4; i++) {
        if (Move::squaresToEdge[fromSquare][i] < 2)
            continue;
        int auxSquare = fromSquare + 2 * Move::directionsOffsets[i];
        int directionsIndex[2];
        if (i == 0 || i == 1) { directionsIndex[0] = 2; directionsIndex[1] = 3; }
        else { directionsIndex[0] = 0; directionsIndex[1] = 1; }
        
        for (int j = 0; j < 2; j++) {
            if (Move::squaresToEdge[auxSquare][directionsIndex[j]] == 0)
                continue;
            int toSquare = auxSquare + Move::directionsOffsets[directionsIndex[j]];
            if (board.squares[toSquare] == Piece::None ||
                Piece::get_piece_color(board.squares[toSquare]) != Piece::get_piece_color(piece))
                moves.push_back(toSquare);
        }
    }
}

void Move::generate_king_moves(Board &board, int piece, int fromSquare, std::list<int> &moves, bool handleCheck) {
    int pieceColor = Piece::get_piece_color(piece);
    for (int i = 0; i < 8; i++) {
        if (Move::squaresToEdge[fromSquare][i] == 0)
            continue;

        int toSquare = fromSquare + Move::directionsOffsets[i];
        if (board.squares[toSquare] == Piece::None ||
            Piece::get_piece_color(board.squares[toSquare]) != pieceColor)
            moves.push_back(fromSquare + Move::directionsOffsets[i]);
    }
    
    // castling
    int colorIndex = pieceColor == Piece::White ? Move::whiteIndex : Move::blackIndex;
    // king square must be 60
    int defaultKingSquare = pieceColor == Piece::White ? 60 : 4;
    if (!Move::kingHasMoved[colorIndex] && fromSquare == defaultKingSquare) {
        if ((handleCheck && !is_in_check(board)) || !handleCheck) {
            if (!Move::rookShortCastleHasMoved[colorIndex]) {
                bool canCastle = true;
                for (int i = 1; i < 3; i++) {
                    if (board.squares[fromSquare + i] != Piece::None)
                        canCastle = false;
                }
                if (canCastle) {
                    moves.push_back(fromSquare + 2);
                }
            }
            if (!Move::rookLongCastleHasMoved[colorIndex]) {
                bool canCastle = true;
                for (int i = 1; i < 4; i++) {
                    if (board.squares[fromSquare - i] != Piece::None)
                        canCastle = false;
                }
                if (canCastle) {
                    moves.push_back(fromSquare - 2);
                }
            }
        }
    }
}

void Move::generate_sliding_moves(Board &board, int piece, int fromSquare, std::list<int> &moves) {
    int startDirectionIndex = Piece::get_piece_type(piece) == Piece::Bishop ? 4 : 0;
    int endDirectionIndex = Piece::get_piece_type(piece) == Piece::Rook ? 4 : 8;

    for (int directionIndex = startDirectionIndex; directionIndex < endDirectionIndex; directionIndex++) {
        for (int i = 0; i < Move::squaresToEdge[fromSquare][directionIndex]; i++) {
            int toSquare = fromSquare + Move::directionsOffsets[directionIndex] * (i + 1);
            int pieceOnToSquare = board.squares[toSquare];
            int pieceColor = Piece::get_piece_color(piece);

            if (Piece::is_color(pieceOnToSquare, pieceColor))
                break;

            moves.push_back(toSquare);

            if (pieceOnToSquare != Piece::None && !Piece::is_color(pieceOnToSquare, pieceColor))
                break;
        }
    }
}

std::string generate_fen_from_position(int board[64]) {
    std::string fen = "";
    int emptyCount = 0;
    
    for (int i = 0; i < 64; i++) {
        int piece = board[i];
        if (piece == Piece::None) {
            emptyCount++;
        } else {
            if (emptyCount > 0) {
                fen += std::to_string(emptyCount);
                emptyCount = 0;
            }
            char pieceChar = ' ';
            switch (Piece::get_piece_type(piece)) {
                case Piece::Rook: pieceChar = 'r'; break;
                case Piece::Knight: pieceChar = 'n'; break;
                case Piece::Bishop: pieceChar = 'b'; break;
                case Piece::Queen: pieceChar = 'q'; break;
                case Piece::King: pieceChar = 'k'; break;
                case Piece::Pawn: pieceChar = 'p'; break;
            }
            if (Piece::get_piece_color(piece) == Piece::White) {
                pieceChar = toupper(pieceChar);
            }
            fen += pieceChar;
        }
        if (i % 8 == 7) {
            if (emptyCount > 0) {
                fen += std::to_string(emptyCount);
                emptyCount = 0;
            }
            if (i != 63) {
                fen += "/";
            }
        }
    }

    return fen;
}

// nao pode rockar quando tiver em cheque
void Move::remove_illegal_moves(Board &board, std::unordered_map<int, std::list<int>> &possibleMoves) {
    // for each possible move, simulate the move, check all opponent responses and remove it if the king can be captured

    for (int fromSquare = 0; fromSquare < 64; fromSquare++) {
        std::list<int> movesToRemove;
        for (int toSquare : possibleMoves[fromSquare]) {
            std::string fen = generate_fen_from_position(board.squares);
            Board virtualBoard = Board(fen);
            int piece = virtualBoard.squares[fromSquare];
            virtualBoard.colorToMove = board.colorToMove;

            virtualBoard.make_move(fromSquare, toSquare, piece, possibleMoves[fromSquare], true);
            bool shortCastle = false;
            bool longCastle = false;
            if (Piece::get_piece_type(piece) == Piece::King && toSquare - fromSquare == 2) {
                shortCastle = true;
            }
            else if (Piece::get_piece_type(piece) == Piece::King && toSquare - fromSquare == -2) {
                longCastle = true;
            }
            std::unordered_map<int, std::list<int>> possibleMovesAhead;
            Move::generate_moves(virtualBoard, possibleMovesAhead, false);
            bool kingCanBeCaptured = false;
            int kingSquare = -1;
            for (int i = 0; i < 64; i++) {
                if (virtualBoard.squares[i] == (Piece::King | board.colorToMove)) {
                    kingSquare = i;
                    break;
                }
            }
            for (int i = 0; i < 64; i++) {
                for (int move : possibleMovesAhead[i]) {
                    if (move == kingSquare || (shortCastle && move == kingSquare - 1) || (longCastle && move == kingSquare + 1)) {
                        kingCanBeCaptured = true;
                        break;
                    }
                }
            }

            if (kingCanBeCaptured) {
                movesToRemove.push_back(toSquare);
            }
        }
        for (int move : movesToRemove) {
            possibleMoves[fromSquare].remove(move);
        }
    }
}

int is_in_check(Board board) {
    std::unordered_map<int, std::list<int>> possibleMoves;
    int kingSquare = -1;
    for (int i = 0; i < 64; i++) {
        if (board.squares[i] == (Piece::King | board.colorToMove)) {
            kingSquare = i;
            break;
        }
    }
    board.colorToMove = board.colorToMove == Piece::White ? Piece::Black : Piece::White;

    Move::generate_moves(board, possibleMoves, false);

    for (int i = 0; i < 64; i++) {
        for (int move : possibleMoves[i]) {
            if (move == kingSquare) {
                return 1;
            }
        }
    }

    board.colorToMove = board.colorToMove == Piece::White ? Piece::Black : Piece::White;
    
    return 0;
}
