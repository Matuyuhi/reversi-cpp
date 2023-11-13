//
// Created by matuyuhi on 2023/11/13.
//

#ifndef REVERSIAI_H
#define REVERSIAI_H

#include "ReversiGame.h"
#include "../header/ReversiBoard.h"

class ReversiAI {
private:
    ReversiBoard&board;
    stone myColor;

    static int evaluateBoard(ReversiBoard&board);


    int minimax(ReversiBoard&board, int depth, int alpha, int beta, bool maximizingPlayer);

public:
    ReversiAI(ReversiBoard&board, stone color) : board(board), myColor(color) {
    }

    std::pair<int, int> chooseMove();
};


#endif //REVERSIAI_H
