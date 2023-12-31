//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK01_REVERSIBOARD_H
#define WORK01_REVERSIBOARD_H

#include <vector>
#include <map>
#include <queue>

#include "Stone.h"
#include "List.h"
#include "cout.h"

/// リバーシのボードを管理するクラス
class ReversiBoard
{
    /// ボードのサイズ
    int boardSize;

    /// 置けるマスの一覧
    std::map<stone, List<std::pair<int, int>>> placeableCells;

    /// 直後にひっくり返したマス
    std::map<stone, std::pair<int, int>> flippedCells;

    std::deque<std::pair<std::pair<int, int>, List<std::pair<int, int>>>> moveHistory;

    /// 8方向のベクトル
    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},
        {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };


    /// 指定方向に石を置けるかをチェックするメソッド
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    /// @param dir 方向
    /// @return 指定方向に石を置けるかどうかの判定
    bool isCanPlaceWithDirection(int row, int col, stone color, std::pair<int, int> dir) const;

    /// 現在のリストを再度確認するメソッド
    /// @param list 確認するリスト
    /// @param color 石の色
    void updatePlaceableCellsWithList(List<std::pair<int, int>> list, stone color);

    /// 指定マスが置けるかどうかをチェックするメソッド
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    void updatePlaceableCell(int row, int col, stone color);

protected:
    /// ボードの初期化処理
    /// 中心に石配置+そのほかの準備
    void Initialized();

    /// ボード
    std::vector<std::vector<stone>> board;

    /// 指定Stoneの置けるマス一覧を更新するメソッド
    /// @param row 置いたマスの行番号 0~(n-1)
    /// @param col 置いたマスの列番号 0~(n-1)
    void updatePlaceableCellsWithStone(int row, int col, List<std::pair<int, int>>& cash);

public:
    /// デフォルトの8マスで初期化するメソッド
    ReversiBoard();

    /// ボードのサイズを指定してボードを初期化するコンストラクタ
    /// @param size ボードのサイズ
    explicit ReversiBoard(int size);

    ReversiBoard(const ReversiBoard& reversiBoard);

    ReversiBoard& operator=(const ReversiBoard& other)
    {
        if (this != &other)
        {
            boardSize = other.boardSize;
            placeableCells = other.placeableCells;
            flippedCells = other.flippedCells;
            board = other.board;
            moveHistory = other.moveHistory;
        }
        return *this;
    }

    /// 指定されたマスに石を置くメソッド
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    /// @param isUpdate 置いた後におけるマスを更新するかどうか
    /// @return 石を置けたかどうか
    bool placeStone(int row, int col, stone color, bool isUpdate = true);

    /// 直前に行った手を一つ戻すメソッド\n
    /// 注意して使う
    bool undoPlaceStone();

    /// 指定されたマスに石を置けるかどうかをチェックするメソッド
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    /// @return 石を置けるかどうか
    bool isCanPlace(int row, int col, stone color) const;

    /// 指定方向の石をひっくり返すメソッド
    /// チェック済前提
    /// @param row 置くマスの行番号 0~(n-1)
    /// @param col 置くマスの列番号 0~(n-1)
    /// @param color 置く石の色
    /// @param dir 方向
    /// @return 返した石の一覧
    List<std::pair<int, int>> flip(int row, int col, stone color, std::pair<int, int> dir);


    /// 現在のボードを出力するメソッド
    /// @param mine 自分の石の色
    void printBoard(stone mine);

    stone getStoneAt(const int row, const int col) const { return board[row][col]; }

    /// 配置可能なマスの一覧を取得するメソッド
    /// @param color 石の色
    /// @return 配置可能なマスの一覧
    List<std::pair<int, int>> getPlaceableCells(stone color);

    /// 配置されている石の数を取得するメソッド
    /// @param color 石の色
    /// @return 石の数
    int getStoneCount(stone color) const;

    /// 配置されている石ごとの数を取得するメソッド
    /// @return 石ごとの数
    std::map<stone, int> getStonesCount() const;

    /// 現在の色リストを再度確認するメソッド
    /// @param color 石の色
    void updatePlaceableCellsInList(stone color);

    /// 指定の石の一覧を取得するメソッド
    /// @return 石のリスト
    List<std::pair<int, int>> getStoneList(stone color) const;

    /// ゲームの終了条件
    /// @return ゲームが終了したかどうか
    bool finished();


    /**
     * @brief ボードサイズを取得するメソッド
     * @return ボードサイズ
     */
    int getBoardSize() const { return boardSize; }

    /// 探索する８方向を返す
    /// @return 8方向のベクトル
    std::vector<std::pair<int, int>> getDirections() const { return directions; }

    /// ボードのコピーを返す
    /// @return ボードのコピー
    ReversiBoard copy() const
    {
        auto copied = ReversiBoard(*this);
        return copied;
    }
};


#endif //WORK01_REVERSIBOARD_H
