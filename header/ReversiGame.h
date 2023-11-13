//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK_REVERSIGAME_H
#define WORK_REVERSIGAME_H


#include "ReversiBoard.h"
#include "../header/AIPlayer.h"
#include "../header/InputPlayer.h"
#include "ReversiAI.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <format>

#include "IPlayer.h"

/// リバーシのプレイを管理するクラス
class ReversiGame : ReversiBoard {
private:
    static constexpr stone playerStone = stone::Black;
    static constexpr int boardSize = 8;

    std::map<stone, IPlayer *> players;
    // bool placeStone(stone color);

    //void autoPlay(int residueCount, stone startColor);

    void gameFinished();

public:
    ReversiGame();

    void Start();
};


#endif //WORK_REVERSIGAME_H
