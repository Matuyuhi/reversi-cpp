//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK_REVERSIGAME_H
#define WORK_REVERSIGAME_H


#include "ReversiBoard.h"
#include "../header/AIPlayer.h"
#include "../header/InputPlayer.h"
#include <iostream>
#include <format>

/// リバーシのプレイを管理するクラス
class ReversiGame : ReversiBoard {
private:
    static constexpr stone playerStone = stone::Black;
    static constexpr int boardSize = 8;

    std::map<stone, IPlayer *> players;


    void gameFinished();

    static void spacer() { std::cout << "//----------------//" << std::endl; }


public:
    /// 終了時のステータス
    enum class FinishedState {
        Restart,
        Quit,
        Error
    };

    ReversiGame();

    /// 再スタート前の初期化処理
    void Clear();

    /// ゲームを開始するメソッド
    /// @return 終了ステータス
    FinishedState Start();

    ~ReversiGame();
};


#endif //WORK_REVERSIGAME_H
