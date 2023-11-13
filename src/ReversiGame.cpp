﻿//
// Created by matuyuhi on 2023/11/08.
//

#include "../header/ReversiGame.h"


ReversiGame::ReversiGame(): ReversiBoard(boardSize) {
    /* ゲーム開始時の4つの石を配置 */
    board[boardSize / 2 - 1][boardSize / 2 - 1] = stone::White;
    board[boardSize / 2 - 1][boardSize / 2] = stone::Black;
    board[boardSize / 2][boardSize / 2 - 1] = stone::Black;
    board[boardSize / 2][boardSize / 2] = stone::White;
    /* 配置した位置に対しておける位置を更新 */
    updatePlaceableCells(boardSize / 2 - 1, boardSize / 2 - 1);
    updatePlaceableCells(boardSize / 2 - 1, boardSize / 2);
    updatePlaceableCells(boardSize / 2, boardSize / 2 - 1);
    updatePlaceableCells(boardSize / 2, boardSize / 2);

    players[playerStone] = new InputPlayer(this, stone::Black);
    players[!playerStone] = new AIPlayer(this, stone::White);
}

void ReversiGame::Start() {
    srand(time(nullptr));
    // 現在のプレイヤーを黒に設定する
    stone currentPlayer = stone::Black;
    printBoard(currentPlayer);

    while (!finished()) {
        std::cout << "//----------------//" << std::endl;
        std::cout << "//----------------//" << std::endl;
        std::cout << "//* " << (currentPlayer == playerStone ? "あなた" : "相手") << "のターン *//" << std::endl;
        TurnState state = players[currentPlayer]->make();
        if (state == TurnState::Quit) {
            std::cout << "ゲームを強制終了します" << std::endl;
            break;
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
        }
        else if (getStoneCount(stone::Black) < getStoneCount(stone::White)) {
            std::cout << "あなたの負けです" << std::endl;
        }
        else {
            std::cout << "引き分けです" << std::endl;
        }
    }
    else if (getPlaceableCells(stone::Black).size() == 0 || getPlaceableCells(stone::White).size() == 0) {
        // 空きマスがある場合(異常終了)
        std::cout << "おけるマスがないため、ゲームを終了します" << std::endl;
        if (getStoneCount(stone::Black) > getStoneCount(stone::White)) {
            std::cout << "あなたの勝ちです" << std::endl;
        }
        else if (getStoneCount(stone::Black) < getStoneCount(stone::White)) {
            std::cout << "あなたの負けです" << std::endl;
        }
        else {
            std::cout << "引き分けです" << std::endl;
        }
    }
    std::cout << "//----------------//" << std::endl;
    std::map<stone, int> stonesCount = getStonesCount();
    std::cout << std::format("{:<21} {}\n", "プレイヤーの石の数:", stonesCount[stone::Black]);
    std::cout << std::format("{:<21} {}\n", "COMの石の数:", stonesCount[stone::White]);
    std::cout << "//----------------//" << std::endl;
}

