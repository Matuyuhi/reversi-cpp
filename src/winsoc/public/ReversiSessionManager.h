//
// Created by yuhi on 1/1/2024.
//

#ifndef WORK_REVERSISESSIONMANAGER_H
#define WORK_REVERSISESSIONMANAGER_H
#include "../../reversi/public/ReversiBoard.h"

namespace winsoc
{
    class ReversiSessionManager : ReversiBoard
    {
    private:
        static constexpr int boardSize = 8;

    public:
        ReversiSessionManager(): ReversiBoard(boardSize)
        {
            Initialized();
        }

        void Initialized()
        {
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

        void SetStone(int row, int col, const stone color)
        {
            placeStone(row, col, color);
        }

        void PrintBoard()
        {
            printBoard(stone::Empty);
        }

        bool IsCanPlaceStone(int row, int col, const stone color) const
        {
            return isCanPlace(row, col, color);
        }

        bool finished()
        {
            return ReversiBoard::finished();
        }

        int getStoneCount(stone color)
        {
            return ReversiBoard::getStoneCount(color);
        }

        List<std::pair<int, int>> getPlaceableCells(stone color) {
            return ReversiBoard::getPlaceableCells(color);
        }
        std::map<stone, int> getStonesCount()
        {
            return ReversiBoard::getStonesCount();
        }
    };
} // winsoc

#endif //WORK_REVERSISESSIONMANAGER_H
