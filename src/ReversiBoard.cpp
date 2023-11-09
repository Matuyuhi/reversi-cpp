//
// Created by matuyuhi on 2023/11/08.
//

#include "../header/ReversiBoard.h"


ReversiBoard::ReversiBoard() : boardSize(8) {
    Initialized();
}

ReversiBoard::ReversiBoard(int size) {
    boardSize = size;
    Initialized();
}

void ReversiBoard::Initialized() {
    /* boardの初期化 */
    board = std::vector<std::vector<stone>>(boardSize, std::vector<stone>(boardSize, stone::Empty));
    placeableCells[stone::Empty].clear();
    placeableCells[stone::Black].clear();
    placeableCells[stone::White].clear();

    flippedCells[stone::Black] = {-1, -1};
    flippedCells[stone::White] = {-1, -1};
}

bool ReversiBoard::placeStone(int row, int col, stone color) {
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize || board[row][col] != stone::Empty) {
        return false;
    }


    bool isFlip = false;
    for (const std::pair<int, int> &dir: directions) {
        if (isCanPlaceWithDirection(row, col, color, dir)) {
            flip(row, col, color, dir);
            isFlip = true;
        }
    }
    if (isFlip) {
        board[row][col] = color;
        flippedCells[color] = {row, col};
//        updatePlaceableCells(row, col);
        updatePlaceableCellsInList(color);
        updatePlaceableCellsInList(!color);
        return true;
    }

    return false;
}

void ReversiBoard::updatePlaceableCellsWithList(List<std::pair<int, int>> list, stone color) {
    for (int i = 0; i < placeableCells[color].size(); ++i) {
        std::pair<int, int> p = list[i];
        if (!isCanPlace(p.first, p.second, color)) {
            placeableCells[color].remove({p.first, p.second});
        }
    }
}

void ReversiBoard::updatePlaceableCellsInList(stone color) {
    List<std::pair<int, int>> list = getStoneList(color);
    for (int i = 0; i < list.size(); ++i) {
        std::pair<int, int> p = list[i];
        updatePlaceableCells(p.first, p.second);
    }
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
//            updatePlaceableCells(row, col);
            continue;
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
    int r;
    int c;
    for (const std::pair<int, int> &dir: directions) {
        r = row + dir.first;
        c = col + dir.second;
        if (r < 0 || r >= boardSize || c < 0 || c >= boardSize) {
            continue;
        }
        updatePlaceableCell(r, c, color);
    }
}

void ReversiBoard::updatePlaceableCell(int row, int col, stone color) {
    bool isCanPlaced = false;
    for (const std::pair<int, int> &updateDir: directions) {
        if (isCanPlaceWithDirection(row, col, color, updateDir)) {
            isCanPlaced = true;
            break;
        }
    }

    if (isCanPlaced && board[row][col] == stone::Empty) {
        if (!placeableCells[color].contains({row, col})) {
            placeableCells[color].add({row, col});
        }
    } else {
        placeableCells[color].remove({row, col});
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

    for (const std::pair<int, int> &dir: directions) {
        if (isCanPlaceWithDirection(row, col, color, dir)) {
            return true;
        }
    }

    return false;
}

void ReversiBoard::printBoard(stone mine) {
    for (int col = 0; col < board.size(); ++col) {
        /// column number
        if (col == 0) {
            std::cout << "  ";
            for (int i = 0; i < board.size(); ++i) {
                std::cout << i + 1 << " ";
            }
            std::cout << std::endl;
        }
        for (int row = 0; row < board[col].size(); ++row) {
            /// row number
            if (row == 0) {
                std::cout << col + 1 << " ";
            }

            /// print board
            std::cout << GREEN_BG;
            switch (board[row][col]) {
                case stone::Empty: {
                    if (placeableCells[mine].contains({row, col})) {
                        std::cout << RED_BG ;
                    }
                    std::cout << ". ";
                    break;
                }
                case stone::Black: {
                    if (flippedCells[stone::Black] == std::make_pair(row, col)) {
                        std::cout << RESET << MAGENTA_BG;
                    }
                    std::cout << BLACK << "B ";
                    break;
                }
                case stone::White: {
                    if (flippedCells[stone::White] == std::make_pair(row, col)) {
                        std::cout << RESET << MAGENTA_BG;
                    }
                    std::cout << CYAN << "W ";

                    break;
                }
            }
            std::cout << RESET;
        }
        std::cout << RESET << std::endl;
    }
}

List<std::pair<int, int>> ReversiBoard::getPlaceableCells(stone color) {
    return placeableCells[color];
}

int ReversiBoard::getStoneCount(stone color) {
    int count = 0;
    for (const std::vector<stone> &row: board) {
        for (const stone &s: row) {
            if (s == color) {
                count++;
            }
        }
    }
    return count;
}

List<std::pair<int, int>> ReversiBoard::getStoneList(stone color) {
    List<std::pair<int, int>> stoneList;
    for (int col = 0; col < board.size(); ++col) {
        for (int row = 0; row < board[col].size(); ++row) {
            if (board[row][col] == color) {
                stoneList.add({row, col});
            }
        }
    }
    return stoneList;
}

std::map<stone, int> ReversiBoard::getStonesCount() {
    std::map<stone, int> stonesCount;
    stonesCount[stone::Black] = 0;
    stonesCount[stone::White] = 0;
    stonesCount[stone::Empty] = 0;
    for (const std::vector<stone> &row: board) {
        for (const stone &s: row) {
            stonesCount[s]++;
        }
    }
    return stonesCount;
}


bool ReversiBoard::finished() {
    return
    /// 空きマスがあるかどうか
    getStoneCount(stone::Empty) == 0 ||
    /// 黒が置けるかどうか
    getPlaceableCells(stone::Black).size() == 0 ||
    /// 白が置けるかどうか
    getPlaceableCells(stone::White).size() == 0;
}