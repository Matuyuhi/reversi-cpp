//
// Created by yuhi on 1/1/2024.
//

#include "../public/ReversiSessionManager.h"

namespace winsoc
{
    void ReversiSessionManager::Init() {
        ReversiBoard::Initialized();

        /* ゲーム開始時の4つの石を配置 */
        board[boardSize / 2 - 1][boardSize / 2 - 1] = stone::White;
        board[boardSize / 2 - 1][boardSize / 2] = stone::Black;
        board[boardSize / 2][boardSize / 2 - 1] = stone::Black;
        board[boardSize / 2][boardSize / 2] = stone::White;

        /* 配置した位置に対しておける位置を更新 */
        updatePlaceableCellsInList(stone::White);
        updatePlaceableCellsInList(stone::Black);
    }

    ReversiSessionManager::ReversiSessionManager() : ReversiBoard(boardSize)
    {
        Init();
    }

    void ReversiSessionManager::SetStone(int row, int col, const stone color) {
        placeStone(row, col, color);
    }

    void ReversiSessionManager::PrintBoard(stone color) {
        printBoard(color);
    }

    int ReversiSessionManager::getStoneCount(stone color) {
        return ReversiBoard::getStoneCount(color);
    }

    std::map<stone, int> ReversiSessionManager::getStonesCount() {
        return ReversiBoard::getStonesCount();
    }
} // winsoc
