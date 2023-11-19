//
// Created by matuyuhi on 2023/11/13.
//

#include "../header/ReversiAI.h"

std::pair<int, int> ReversiAI::chooseMove() {
    List<std::pair<int, int>> placeable = board.getPlaceableCells(myColor);

    std::pair<int, int> bestMove;
    int bestScore = INT_MIN;


    const unsigned int numCores = std::thread::hardware_concurrency();
    const unsigned int numTasks = placeable.size();
    unsigned int batchSize = numTasks / numCores;

    // 最小サイズ
    constexpr unsigned int minBatchSize = 1;

    batchSize = std::max(minBatchSize, batchSize);

    // タスクの総数がCPUコア数より少ない場合、batchSizeを1に設定
    if (numTasks < numCores) {
        batchSize = 1;
    }

    for (int start = 0; start < numTasks; start += batchSize) {
        int end = std::min(numTasks, start + batchSize);
        std::vector<std::future<std::pair<int, std::pair<int, int>>>> futures;

        auto it = placeable.begin();
        std::advance(it, start);  // イテレータをstartの位置まで進める

        for (int i = start; i < end && it != placeable.end(); ++i, ++it) {
            std::pair<int, int> cell = *it;
            futures.push_back(std::async(std::launch::async, [cell, this] {
                ReversiBoard boardCopy = board.copy();
                boardCopy.placeStone(cell.first, cell.second, myColor);
                int score = minimax(boardCopy, depth, INT_MIN, INT_MAX, false);
                return std::make_pair(score, cell);
            }));
        }

        for (auto& f : futures) {
            auto result = f.get();
            int score = result.first;
            if (score >= bestScore) {
                if (score == bestScore && rand() % 2 == 0) continue;
                bestScore = score;
                bestMove = result.second;
            }
        }
    }

    return bestMove;
}

int ReversiAI::evaluateBoard(ReversiBoard&board) const {
    int score = 0;
    const int boardSize = board.getBoardSize();

    // 角の占有は高い価値を持つ
    const std::vector<std::pair<int, int>> corners = {
        {0, 0}, {0, boardSize - 1}, {boardSize - 1, 0}, {boardSize - 1, boardSize - 1}
    };
    for (const std::pair<int, int>&corner: corners) {
        const stone cornerStone = board.getStoneAt(corner.first, corner.second);
        if (cornerStone == stone::Black) score += 200;
        else if (cornerStone == stone::White) score -= 200;
    }

    // 角の近くのセルに対するスコア調整
    const std::vector<std::pair<int, int>> adjacentToCorners = {
        {0, 1}, {1, 0}, {1, 1},
        {0, boardSize - 2}, {1, boardSize - 1}, {1, boardSize - 2},
        {boardSize - 2, 0}, {boardSize - 1, 1}, {boardSize - 2, 1},
        {boardSize - 1, boardSize - 2}, {boardSize - 2, boardSize - 1}, {boardSize - 2, boardSize - 2}
    };

    for (const std::pair<int, int>&pos: adjacentToCorners) {
        stone adjStone = board.getStoneAt(pos.first, pos.second);
        if (adjStone == stone::Black) score -= 100;
        else if (adjStone == stone::White) score += 100;
    }

    // 辺のセルのスコア加算
    for (int i = 0; i < boardSize; ++i) {
        std::vector<std::pair<int, int>> edges = {{0, i}, {i, 0}, {boardSize - 1, i}, {i, boardSize - 1}};
        for (const std::pair<int, int>&edge: edges) {
            stone edgeStone = board.getStoneAt(edge.first, edge.second);
            if (edgeStone == stone::Black) score += 10;
            else if (edgeStone == stone::White) score -= 10;
        }
    }

    // ボード上の全ての石を評価
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            const stone currentStone = board.getStoneAt(row, col);
            if (currentStone == stone::Black) score += 5;
            else if (currentStone == stone::White) score -= 5;
        }
    }

    // 相手の移動可能性を減らすことで得られるスコア
    const int myMoves = board.getPlaceableCells(myColor).size();
    const int otherMoves = board.getPlaceableCells(!myColor).size();

    score += (myMoves - otherMoves) * 50;

    // 相手が動けなければ高得点
    if (otherMoves == 0) {
        score += 500;
    }

    return score;
}

int ReversiAI::minimax(ReversiBoard&board, const int depth, int alpha, int beta, bool maximizingPlayer) {
    // ボードの評価
    if (depth == 0 || board.finished()) {
        return evaluateBoard(board);
    }

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        const List<std::pair<int, int>> list = board.getPlaceableCells(!myColor);
        for (const std::pair<int, int>&move: list) {
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
        const List<std::pair<int, int>> list = board.getPlaceableCells(myColor);
        for (const std::pair<int, int>&move: list) {
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
