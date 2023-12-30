//
// Created by matuyuhi on 2023/11/13.
//

#include "../header/AIPlayer.h"

TurnState AIPlayer::make() {
    if (reversiBoard->getPlaceableCells(color).size() == 0) {
        std::cout << "おけるマスがありません" << '\n';
        return TurnState::Defalut;
    }
    ReversiAI ai = ReversiAI(*reversiBoard, color);

    // ランダムに置ける場所を探す
    const std::pair<int, int> cell = ai.chooseMove();
    // 石を置く
    if (reversiBoard->placeStone(cell.first, cell.second, color)) {
        std::cout << "(縦: " << cell.second + 1 << ", 横: " << cell.first + 1 << ")に";
        std::cout << "石を置きました" << '\n';
    }
    return TurnState::Defalut;
}
