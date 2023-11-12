//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK_REVERSIGAME_H
#define WORK_REVERSIGAME_H


#include "ReversiBoard.h"
#include <sstream>
#include <cstdlib>
#include <ctime>

/// リバーシのプレイを管理するクラス
class ReversiGame: ReversiBoard {
private:
    static constexpr int boardSize = 8;
    bool placeStone(stone color);

    void autoPlay(int residueCount, stone startColor);

    static std::pair<int, std::string> getInputNumbers();

    void gameFinished();
public:
    ReversiGame();

    void Start();

};


#endif //WORK_REVERSIGAME_H
