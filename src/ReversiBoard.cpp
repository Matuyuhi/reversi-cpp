//
// Created by matuyuhi on 2023/11/08.
//

#include "../header/ReversiBoard.h"


ReversiBoard::ReversiBoard(): boardSize(8) {
    /* boardの初期化 */
    board = std::vector<std::vector<stone>>(boardSize, std::vector<stone>(boardSize, stone::Empty));
    placeableCells[stone::Empty].clear();
    placeableCells[stone::Black].clear();
    placeableCells[stone::White].clear();

    /* ゲーム開始時の4つの石を配置 */
    board[3][3] = stone::White;
    updatePlaceableCells(3, 3);
    board[3][4] = stone::Black;
    updatePlaceableCells(3, 4);
    board[4][3] = stone::Black;
    updatePlaceableCells(4, 3);
    board[4][4] = stone::White;
    updatePlaceableCells(4, 4);

}

bool ReversiBoard::placeStone(int row, int col, stone color) {
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize || board[row][col] != stone::Empty) {
        return false;
    }


    bool isFlip = false;
    for (const std::pair<int, int>& dir : directions) {
        if (isCanPlaceWithDirection(row, col, color, dir)) {
            flip(row, col, color, dir);
            isFlip = true;
        }
    }
    if (isFlip) {
        board[row][col] = color;
        updatePlaceableCells(row, col);
        return true;
    }

    return false;
}

void ReversiBoard::flip(int row, int col, stone color, std::pair<int, int> dir) {
    while (true) {
        row += dir.first;
        col += dir.second;
        if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
            break;
        }
        if (board[row][col] == stone::Empty) {
            break;
        }
        if (board[row][col] == !color) {
            board[row][col] = color;
            updatePlaceableCells(row, col);
        }

        if (board[row][col] == color) {
            break;
        }
    }
}

void ReversiBoard::updatePlaceableCells(int row, int col) {
    updatePlaceableCellsWithStone(row, col, stone::Black);
    updatePlaceableCellsWithStone(row, col, stone::White);
}

void ReversiBoard::updatePlaceableCellsWithStone(int row, int col, stone color) {
    int r = row;
    int c = col;
    for (const std::pair<int, int>& dir : directions) {
        r = row + dir.first;
        c = col + dir.second;
        bool isCanPlaced = false;
        for (const std::pair<int, int>& updateDir : directions) {
            if (isCanPlaceWithDirection(r, c, color, updateDir)) {
                isCanPlaced = true;
                break;
            }
        }

        if (isCanPlaced) {
            placeableCells[color].add({r, c});
        } else {
            placeableCells[color].remove({r, c});
        }


    }
}

bool ReversiBoard::isCanPlaceWithDirection(int row, int col, stone color, std::pair<int, int> dir) {
    int canPlaceCount = 0;
    while (true) {
        row += dir.first;
        col += dir.second;
        if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
            return false;
        }
        if (board[row][col] == stone::Empty) {
            return false;
        }
        if (board[row][col] == !color) {
            canPlaceCount++;
            continue;
        }

        if (board[row][col] == color) {
            return canPlaceCount > 0;
        }
    }
}

bool ReversiBoard::isCanPlace(int row, int col, stone color) {
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize || board[row][col] != stone::Empty) {
        return false;
    }

    for (const std::pair<int, int>& dir : directions) {
        if (isCanPlaceWithDirection(row, col, color, dir)) {
            return true;
        }
    }

    return false;
}

void ReversiBoard::printBoard(stone mine) {
    for (int col = 0; col < board.size(); ++col) {
        for (int row = 0; row < board[col].size(); ++row) {
            switch (board[row][col]) {
                case stone::Empty: {
                    if (placeableCells[mine].contains({row, col})) {
                        std::cout << RED << "O " << RESET;
                    } else {
                        std::cout << ". ";
                    }
                    break;
                }
                case stone::Black: std::cout << "B "; break;
                case stone::White: std::cout << "W "; break;
            }
        }
        std::cout << std::endl;
    }
}
