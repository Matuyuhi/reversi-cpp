//
// Created by yuhi on 1/1/2024.
//

#ifndef WORK_REVERSISESSIONMANAGER_H
#define WORK_REVERSISESSIONMANAGER_H
#include "../../reversi/public/ReversiBoard.h"

namespace winsoc
{
    class ReversiSessionManager : public ReversiBoard
    {
    private:
        static constexpr int boardSize = 8;

    public:
        ReversiSessionManager();

        void Init();

        void SetStone(int row, int col, const stone color);

        void PrintBoard(stone color = stone::Empty);


        int getStoneCount(stone color) const;

        std::map<stone, int> getStonesCount() const;
    };
} // winsoc

#endif //WORK_REVERSISESSIONMANAGER_H
