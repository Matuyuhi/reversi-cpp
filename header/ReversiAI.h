//
// Created by matuyuhi on 2023/11/13.

#ifndef REVERSIAI_H
#define REVERSIAI_H

#include "ReversiGame.h"
#include "../header/ReversiBoard.h"

/**
 * @brief 各手を評価して最適な位置を返すクラス
 */
class ReversiAI {
    const stone myColor;
    ReversiBoard&board;

    /// 探索する深度(depth手先まで読む)
    /// 実行するCPUの性能によって変更すると良い
    static constexpr int depth = 2;

    /// 現在のボードを評価する
    /// @return スコア
    int evaluateBoard(ReversiBoard&board) const;


    /// ミニマックス法で最適な手を探索する
    /// @return スコア
    int minimax(ReversiBoard&board, int depth, int alpha, int beta, bool maximizingPlayer);

public:
    ReversiAI(ReversiBoard&board, const stone color) : board(board), myColor(color) {
    }

    /// 最善の手を返す
    /// @return cell
    std::pair<int, int> chooseMove();
};


#endif //REVERSIAI_H
