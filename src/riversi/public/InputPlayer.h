﻿//
// Created by matuyuhi on 2023/11/13.
//

#ifndef INPUTPLAYER_H
#define INPUTPLAYER_H
#include "IPlayer.h"
#include "ReversiAI.h"
#include "TurnState.h"


/// playerの手を入力するクラス
class InputPlayer final : public IPlayer
{
    /// 空いたマスが指定以下で手動に切り替える
    static constexpr int inAutoSwitchCount = 4;

    /// 自動入力モードかどうか
    bool isAuto = false;

public:
    TurnState make() override;

    InputPlayer(ReversiBoard* reversiBoard, const stone color): IPlayer(reversiBoard, color)
    {
    }
};


#endif //INPUTPLAYER_H
