//
// Created by matuyuhi on 2023/11/13.
//

#include "../header/ReversiAI.h"

#include <climits>
#include <future>

std::pair<int, int> ReversiAI::chooseMove() {
    List<std::pair<int, int>> placeable = board.getPlaceableCells(myColor);

    // 各手の評価値を計算するための future を格納するベクター
    std::vector<std::future<std::pair<int, std::pair<int, int>>>> futures;

    // 並列に各手を評価
    for (int i = 0; i < placeable.size(); i++) {
        std::pair<int, int> cell = placeable[i];
        futures.push_back(std::async(std::launch::async, [cell, this] {
            ReversiBoard boardCopy = board.copy(); // ボードのコピーを作成
            boardCopy.placeStone(cell.first, cell.second, myColor);
            int score = minimax(boardCopy, 7, INT_MIN, INT_MAX, false);
            return std::make_pair(score, cell);
        }));
    }

    // 最も良い手を見つける
    std::pair<int, int> bestMove;
    int bestScore = INT_MIN;
    for (auto&f: futures) {
        auto result = f.get(); // 各 future の結果を取得
        int score = result.first;
        std::pair<int, int> cell = result.second;
        if (score >= bestScore) {
            if (score == bestScore) {
                if (rand() % 2 == 0) {
                    bestScore = score;
                    bestMove = cell;
                }
            }
            else {
                bestScore = score;
                bestMove = cell;
            }
        }
    }

    return bestMove;
}

int ReversiAI::evaluateBoard(ReversiBoard&board) {
    int score = 0;
    int boardSize = board.getBoardSize();

    // 角の占有は高い価値を持つ
    std::vector<std::pair<int, int>> corners = {
        {0, 0}, {0, boardSize - 1}, {boardSize - 1, 0}, {boardSize - 1, boardSize - 1}
    };
    for (const auto&corner: corners) {
        stone cornerStone = board.getStoneAt(corner.first, corner.second);
        if (cornerStone == stone::Black) score += 100;
        else if (cornerStone == stone::White) score -= 100;
    }

    // 角の近くのセルに対するスコア調整
    std::vector<std::pair<int, int>> adjacentToCorners = {
        {0, 1}, {1, 0}, {1, 1},
        {0, boardSize - 2}, {1, boardSize - 1}, {1, boardSize - 2},
        {boardSize - 2, 0}, {boardSize - 1, 1}, {boardSize - 2, 1},
        {boardSize - 1, boardSize - 2}, {boardSize - 2, boardSize - 1}, {boardSize - 2, boardSize - 2}
    };

    for (const auto&pos: adjacentToCorners) {
        stone adjStone = board.getStoneAt(pos.first, pos.second);
        if (adjStone == stone::Black) score -= 25;
        else if (adjStone == stone::White) score += 25;
    }

    // 辺のセルのスコア加算
    for (int i = 0; i < boardSize; ++i) {
        std::vector<std::pair<int, int>> edges = {{0, i}, {i, 0}, {boardSize - 1, i}, {i, boardSize - 1}};
        for (const auto&edge: edges) {
            stone edgeStone = board.getStoneAt(edge.first, edge.second);
            if (edgeStone == stone::Black) score += 5;
            else if (edgeStone == stone::White) score -= 5;
        }
    }

    // ボード上の全ての石を評価
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            stone currentStone = board.getStoneAt(row, col);
            if (currentStone == stone::Black) score += 1;
            else if (currentStone == stone::White) score -= 1;
        }
    }

    // 相手の移動可能性を減らすことで得られるスコア
    const int blackMoves = board.getPlaceableCells(stone::Black).size();
    const int whiteMoves = board.getPlaceableCells(stone::White).size();

    score += (whiteMoves - blackMoves) * 10;

    // 他の戦略的要素を加味する場合はここに追加する
    // 例えば、安定した石の数、辺の占有、移動可能性の削減など

    return score;
}

int ReversiAI::minimax(ReversiBoard&board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || board.finished()) {
        return evaluateBoard(board); // ボードの評価
    }

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (const auto&move: board.getPlaceableCells(!myColor)) {
            board.placeStone(move.first, move.second, !myColor);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board.undoPlaceStone();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break; // ベータ枝刈り
        }
        return maxEval;
    }
    else {
        int minEval = INT_MAX;
        for (const auto&move: board.getPlaceableCells(myColor)) {
            board.placeStone(move.first, move.second, myColor);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board.undoPlaceStone();
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break; // アルファ枝刈り
        }
        return minEval;
    }
}
