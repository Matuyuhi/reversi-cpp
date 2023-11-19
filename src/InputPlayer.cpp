//
// Created by matuyuhi on 2023/11/13.
//

#include "../header/InputPlayer.h"
#define INPUT_ERROR_NUMBER 12192438

TurnState InputPlayer::make() {
    if (reversiBoard->getPlaceableCells(color).size() == 0) {
        std::cout << "おけるマスがありません" << std::endl;
        return TurnState::Defalut;
    }
    std::cout << "石を置く場所を入力してください" << std::endl;
    std::cout << "特殊コマンド a: 残り" << inAutoSwitchCount << "マスまで自動入力 q: 終了" << std::endl;
    while (true) {
        int row, col;
        if (!isAuto) {
            while (true) {
                std::cout << "縦(1~8):";
                std::pair<int, std::string> input = getInputNumbers();
                if (input.second == "a") {
                    isAuto = true;
                    break;
                }
                if (input.second == "q") {
                    return TurnState::Quit;
                }
                if (input.first == INPUT_ERROR_NUMBER) {
                    std::cout << "入力が不正です" << std::endl;
                    continue;
                }
                col = input.first - 1;
                break;
            }

            while (!isAuto) {
                std::cout << "横(1~8):";
                std::pair<int, std::string> input = getInputNumbers();
                if (input.second == "a") {
                    isAuto = true;
                    break;
                }
                if (input.second == "q") {
                    return TurnState::Quit;
                }
                if (input.first == INPUT_ERROR_NUMBER) {
                    std::cout << "入力が不正です" << std::endl;
                    continue;
                }
                row = input.first - 1;
                break;
            }
        }
        else {
            if (reversiBoard->getStoneCount(stone::Empty) <= inAutoSwitchCount + 1) {
                isAuto = false;
                std::cout << "残りマスが" << inAutoSwitchCount << "つになったので、手動モードに切り替えます" << std::endl;
                continue;
            }
        }

        if (isAuto) {
            ReversiAI ai = ReversiAI(*reversiBoard, color);

            // ランダムに置ける場所を探す
            const std::pair<int, int> cell = ai.chooseMove();
            // 石を置く
            if (reversiBoard->placeStone(cell.first, cell.second, color)) {
                std::cout << "(縦: " << cell.second + 1 << ", 横: " << cell.first + 1 << ")に";
                std::cout << "石を置きました" << std::endl;
            }
        }
        else {
            // 入力した座標が置ける場所かどうかを確認する
            if (reversiBoard->getPlaceableCells(color).contains({row, col})) {
                // 置ける場合は、石を置く
                reversiBoard->placeStone(row, col, color);
                std::cout << "(縦: " << col + 1 << ", 横: " << row + 1 << ")に";
                std::cout << "石を置きました" << std::endl;
            }
            else {
                // 置けない場合は、もう一度入力させる
                std::cout << "そこには置けません" << std::endl;
                continue;
            }
        }
        break;
    }
    return TurnState::Defalut;
}

std::pair<int, std::string> InputPlayer::getInputNumbers() {
    std::string input;
    std::cin >> input;
    if (std::isdigit(input[0])) {
        // 文字列からint型に変換を試みる
        int number;
        std::stringstream ss(input);
        if (ss >> number) {
            // 変換成功したら、intと元の文字列のペアを返す
            return std::make_pair(number, "");
        }
    }
    // 変換できなければ、文字列のみを返す
    return std::make_pair(INPUT_ERROR_NUMBER, input);
}
