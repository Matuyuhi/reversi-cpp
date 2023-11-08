//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK01_REVERSIBOARD_H
#define WORK01_REVERSIBOARD_H

#include <vector>
#include <iostream>
#include <map>
#include "Stone.h"
#include "List.h"
#include "cout.h"

// リバーシのボードを管理するクラス
class ReversiBoard {
private:
    std::vector<std::vector<stone>> board;
    int boardSize;

    // 置けるマスの一覧
    std::map<stone, List<std::pair<int, int>>> placeableCells;

    /// 8方向のベクトル
    const std::vector<std::pair<int, int>> directions = {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0},
            {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };

    /// 指定Stoneの置けるマス一覧を更新するメソッド
    /// @param row 置いたマスの行番号 0~(n-1)
    /// @param col 置いたマスの列番号 0~(n-1)
    /// @param color 置いた石の色
    void updatePlaceableCellsWithStone(int row, int col, stone color);

    /// おけるマス一覧を更新するメソッド
    /// @param row 置いたマスの行番号 0~(n-1)
    /// @param col 置いたマスの列番号 0~(n-1)
    void updatePlaceableCells(int row, int col);


    /// 指定方向に石を置けるかをチェックするメソッド
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    /// @param dir 方向
    bool isCanPlaceWithDirection(int row, int col, stone color, std::pair<int, int> dir);


public:
    // コンストラクタ
    ReversiBoard();

    /// 指定されたマスに石を置くメソッド
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    /// @return 石を置けたかどうか
    bool placeStone(int row, int col, stone color);

    /// 指定されたマスに石を置けるかどうかをチェックするメソッド
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    bool isCanPlace(int row, int col, stone color);

    /// 指定方向の石をひっくり返すメソッド
    void flip(int row, int col, stone color, std::pair<int, int> dir);


    /// 現在のボードを出力するメソッド
    /// @param mine 自分の石の色
    void printBoard(stone mine);
};


#endif //WORK01_REVERSIBOARD_H
