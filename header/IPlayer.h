//
// Created by matuyuhi on 2023/11/13.
//

#ifndef IPLAYER_H
#define IPLAYER_H

#include "ReversiBoard.h"
#include "Stone.h"
#include "TurnState.h"
#include "Input.h"

class IPlayer {
protected:
    ReversiBoard* reversiBoard;
    stone color;

public:
    virtual ~IPlayer() = default;

    /// 手を打つ
    /// @return ターン終了時のステート
    virtual TurnState make() = 0;

    IPlayer(ReversiBoard* reversiBoard, const stone color) {
        this->reversiBoard = reversiBoard;
        this->color = color;
    }
};

#endif //IPLAYER_H
