//
// Created by matuyuhi on 2023/11/13.
//

#ifndef AIPLAYER_H
#define AIPLAYER_H
#include "IPlayer.h"
#include "ReversiAI.h"
#include "TurnState.h"

/**
 * @brief 相手用のクラス
 */
class AIPlayer final : public IPlayer {
public:
    TurnState make() override;

    AIPlayer(ReversiBoard* reversiBoard, const stone color): IPlayer(reversiBoard, color) {
    }
};


#endif //AIPLAYER_H
