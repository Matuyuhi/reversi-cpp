//
// Created by matuyuhi on 2023/11/13.
//

#ifndef IPLAYER_H
#define IPLAYER_H

#include "ReversiBoard.h"
#include "Stone.h"
#include "TurnState.h"

class IPlayer {
protected:
    ReversiBoard* reversiBoard;
    stone color;

public:
    virtual TurnState make() = 0;

    IPlayer(ReversiBoard* reversiBoard, stone color) {
        this->reversiBoard = reversiBoard;
        this->color = color;
    }
};

#endif //IPLAYER_H
