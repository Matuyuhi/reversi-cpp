//
// Created by matuyuhi on 2023/11/08.
//

#include "../header/ReversiGame.h"

#define INPUT_ERROR_NUMBER 12192438

ReversiGame::ReversiGame(): ReversiBoard(boardSize) {
    /* ゲーム開始時の4つの石を配置 */
    board[boardSize/2-1][boardSize/2-1] = stone::White;
    board[boardSize/2-1][boardSize/2] = stone::Black;
    board[boardSize/2][boardSize/2-1] = stone::Black;
    board[boardSize/2][boardSize/2] = stone::White;
    /* 配置した位置に対しておける位置を更新 */
    updatePlaceableCells(boardSize/2-1, boardSize/2-1);
    updatePlaceableCells(boardSize/2-1, boardSize/2);
    updatePlaceableCells(boardSize/2, boardSize/2-1);
    updatePlaceableCells(boardSize/2, boardSize/2);
}

void ReversiGame::Start() {
    srand(time(nullptr));
    // 現在のプレイヤーを黒に設定する
    stone currentPlayer = stone::Black;
    printBoard(currentPlayer);

    while (!finished()) {
        // 現在のプレイヤーが人間の場合
        if (currentPlayer == stone::Black) {
            // 標準入力から縦横を入力させる
            int row, col;
            bool isAuto = false;
            while(true) {
                std::cout << "縦(1~8):";
                std::pair<int, std::string> input = getInputNumbers();
                if (input.second == "a") {
                    autoPlay(4, currentPlayer);
                    isAuto = true;
                    break;
                } else if (input.first == INPUT_ERROR_NUMBER) {
                    std::cout << "入力が不正です" << std::endl;
                    continue;
                }
                col = input.first - 1;
                break;
            }
            if (isAuto) {
                std::cout << "//----------------//" << std::endl;
                std::cout << "//* あなたのターン *//" << std::endl;
                continue;
            }

            while(true) {
                std::cout << "横(1~8):";
                std::pair<int, std::string> input = getInputNumbers();
                if (input.second == "a") {
                    autoPlay(4, currentPlayer);
                    isAuto = true;
                    continue;
                } else if (input.first == INPUT_ERROR_NUMBER) {
                    std::cout << "入力が不正です" << std::endl;
                    continue;
                }
                row = input.first - 1;
                break;
            }
            if (isAuto) {
                std::cout << "//----------------//" << std::endl;
                std::cout << "//* あなたのターン *//" << std::endl;
                continue;
            }


            // 入力した座標が置ける場所かどうかを確認する
            if (getPlaceableCells(currentPlayer).contains({row, col})) {
                // 置ける場合は、石を置く
                ReversiBoard::placeStone(row, col, currentPlayer);
                std::cout << "石を置きました" << std::endl;
                std::cout << "//----------------//" << std::endl;
                std::cout << "//* 相手のターン  *//" << std::endl;
            } else {
                // 置けない場合は、もう一度入力させる
                std::cout << "そこには置けません" << std::endl;
                continue;
            }
        } else {
            // 現在のプレイヤーがCOMの場合
            // ランダムに石を置く
            placeStone(currentPlayer);
            std::cout << "COMが石を置きました" << std::endl;
            std::cout << "//----------------//" << std::endl;
            std::cout << "//* あなたのターン *//" << std::endl;
        }

        // 次のプレイヤーに交代する
        currentPlayer = !currentPlayer;
        printBoard(currentPlayer);
    }
    gameFinished();
}

void ReversiGame::gameFinished() {
    std::cout << "//----------------//" << std::endl;
    // 結果を出力する
    if (!getStoneCount(stone::Empty)) {
        // 空きマスがない場合(正常な終了)
        if (getStoneCount(stone::Black) > getStoneCount(stone::White)) {
            std::cout << "あなたの勝ちです" << std::endl;
        } else if (getStoneCount(stone::Black) < getStoneCount(stone::White)) {
            std::cout << "あなたの負けです" << std::endl;
        } else {
            std::cout << "引き分けです" << std::endl;
        }
    }
    else if (getPlaceableCells(stone::Black).size() == 0 || getPlaceableCells(stone::White).size() == 0) {
        // 空きマスがある場合(異常終了)
        std::cout << "おけるマスがないため、ゲームを終了します" << std::endl;
        if (getStoneCount(stone::Black) > getStoneCount(stone::White)) {
            std::cout << "あなたの勝ちです" << std::endl;
        } else if (getStoneCount(stone::Black) < getStoneCount(stone::White)) {
            std::cout << "あなたの負けです" << std::endl;
        } else {
            std::cout << "引き分けです" << std::endl;
        }
    }
    std::cout << "//----------------//" << std::endl;
    std::map<stone, int> stonesCount = getStonesCount();
    std::cout << std::format("{:<21} {}\n", "プレイヤーの石の数:", stonesCount[stone::Black]);
    std::cout << std::format("{:<21} {}\n", "COMの石の数:", stonesCount[stone::White]);
    std::cout << "//----------------//" << std::endl;
}

void ReversiGame::autoPlay(int residueCount, stone startColor) {

    stone currentPlayer = startColor;

    while (!finished() && getStoneCount(stone::Empty) > residueCount) {
        // ランダムに石を置く
        placeStone(currentPlayer);
        if (currentPlayer == stone::Black) {
            std::cout << "COMが石を置きました" << std::endl;
            std::cout << "//----------------//" << std::endl;
            std::cout << "//* あなたのターン *//" << std::endl;
        } else {
            std::cout << "石を置きました" << std::endl;
            std::cout << "//----------------//" << std::endl;
            std::cout << "//* 相手のターン  *//" << std::endl;
        }

        // 次のプレイヤーに交代する
        currentPlayer = !currentPlayer;
        printBoard(currentPlayer);
    }
    std::cout << "//----------------//" << std::endl;
}

std::pair<int, std::string> ReversiGame::getInputNumbers() {
    std::string input;
    std::cin >> input;
    if(std::isdigit(input[0])) {
        // 文字列からint型に変換を試みる
        int number;
        std::stringstream ss(input);
        if(ss >> number) {
            // 変換成功したら、intと元の文字列のペアを返す
            return std::make_pair(number, "");
        }
    }
    // 変換できなければ、文字列のみを返す
    return std::make_pair(INPUT_ERROR_NUMBER, input);
}

bool ReversiGame::placeStone(stone color) {
    // ランダムに置ける場所を探す
    List<std::pair<int, int>> placeableCells = getPlaceableCells(color);
    int randomIndex = rand() % placeableCells.size();
    std::pair<int, int> randomCell = placeableCells[randomIndex];
    std::cout << "(縦: " << randomCell.second + 1 << ", 横: " << randomCell.first + 1 << ")に";
    // 石を置く
    if (ReversiBoard::placeStone(randomCell.first, randomCell.second, color)) {
        std::cout << "石を置きました" << std::endl;
    } else {
        std::cout << "石を置けませんでした" << std::endl;
    }
    return true;
}
