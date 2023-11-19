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

ReversiBoard::ReversiBoard(const ReversiBoard&reversiBoard) {
    boardSize = reversiBoard.boardSize;
    board = reversiBoard.board;
    placeableCells = reversiBoard.placeableCells;
    flippedCells = reversiBoard.flippedCells;
    moveHistory = reversiBoard.moveHistory;
}

void ReversiBoard::Initialized() {
    /* boardの初期化 */
    board = std::vector<std::vector<stone>>(boardSize, std::vector<stone>(boardSize, stone::Empty));
    placeableCells[stone::Empty].clear();
    placeableCells[stone::Black].clear();
    placeableCells[stone::White].clear();

    flippedCells[stone::Black] = {-1, -1};
    flippedCells[stone::White] = {-1, -1};

    moveHistory.clear();
}

bool ReversiBoard::placeStone(int row, int col, const stone color, bool isUpdate) {
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize || board[row][col] != stone::Empty) {
        return false;
    }

    List<std::pair<int, int>> flippedStones = List<std::pair<int, int>>();
    bool isFlip = false;
    for (const std::pair<int, int>&dir: directions) {
        if (isCanPlaceWithDirection(row, col, color, dir)) {
            flippedStones += flip(row, col, color, dir);
            isFlip = true;
        }
    }
    if (isFlip) {
        board[row][col] = color;
        flippedCells[color] = {row, col};
        placeableCells[stone::Black].clear();
        placeableCells[stone::White].clear();
        updatePlaceableCellsInList(stone::White);
        updatePlaceableCellsInList(stone::Black);
        moveHistory.push_back({{row, col}, flippedStones});
        return true;
    }

    return false;
}

bool ReversiBoard::undoPlaceStone() {
    if (moveHistory.empty()) {
        return false;
    }

    const std::pair<std::pair<int, int>, List<std::pair<int, int>>> lastMove = moveHistory.back();
    moveHistory.pop_back();

    std::pair<int, int> lastPosition = lastMove.first;
    List<std::pair<int, int>> flippedStones = lastMove.second;

    board[lastPosition.first][lastPosition.second] = stone::Empty;

    for (std::pair<int, int> p: flippedStones) {
        board[p.first][p.second] = !board[p.first][p.second];
    }

    placeableCells[stone::Black].clear();
    placeableCells[stone::White].clear();
    updatePlaceableCellsInList(stone::White);
    updatePlaceableCellsInList(stone::Black);
    // updatePlaceableCellsWithList(placeableCells[stone::White], stone::White);
    // updatePlaceableCellsWithList(placeableCells[stone::Black], stone::Black);
    return true;
}


void ReversiBoard::updatePlaceableCellsWithList(List<std::pair<int, int>> list, const stone color) {
    for (const std::pair<int, int>& p : placeableCells[color]) {
        if (!isCanPlace(p.first, p.second, color)) {
            placeableCells[color].remove({p.first, p.second});
        }
    }
}

void ReversiBoard::updatePlaceableCellsInList(stone color) {
    List<std::pair<int, int>> list = getStoneList(color);
    List<std::pair<int, int>> cash = List<std::pair<int, int>>();
    for (const std::pair<int, int>& p : list) {
        updatePlaceableCellsWithStone(p.first, p.second, cash);
    }
}

List<std::pair<int, int>> ReversiBoard::flip(int row, int col, const stone color, std::pair<int, int> dir) {
    List<std::pair<int, int>> flippedStones = List<std::pair<int, int>>();
    flippedStones.clear();
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
            flippedStones.add({row, col});
            continue;
        }

        if (board[row][col] == color) {
            break;
        }
    }
    return flippedStones;
}

void ReversiBoard::updatePlaceableCellsWithStone(const int row, const int col, List<std::pair<int, int>>& cash) {
    for (const std::pair<int, int>&dir: directions) {
        const int r = row + dir.first;
        const int c = col + dir.second;
        if (cash.contains({r, c})) {
            continue;
        }
        if (r < 0 || r >= boardSize || c < 0 || c >= boardSize) {
            continue;
        }
        updatePlaceableCell(r, c, stone::Black);
        updatePlaceableCell(r, c, stone::White);
        cash.add({r, c});
    }
}

void ReversiBoard::updatePlaceableCell(int row, int col, const stone color) {
    bool isCanPlaced = false;
    for (const std::pair<int, int>&updateDir: directions) {
        if (isCanPlaceWithDirection(row, col, color, updateDir)) {
            isCanPlaced = true;
            break;
        }
    }

    if (isCanPlaced && board[row][col] == stone::Empty) {
        placeableCells[color].add({row, col});
    }
    else {
        placeableCells[color].remove({row, col});
    }
}

bool ReversiBoard::isCanPlaceWithDirection(int row, int col, const stone color, std::pair<int, int> dir) const {
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

bool ReversiBoard::isCanPlace(const int row, const int col, const stone color) const {
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize || board[row][col] != stone::Empty) {
        return false;
    }

    for (const std::pair<int, int>&dir: directions) {
        if (isCanPlaceWithDirection(row, col, color, dir)) {
            return true;
        }
    }

    return false;
}

void ReversiBoard::printBoard(const stone mine) {
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
                        std::cout << CYAN_BG;
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
                    std::cout << RED << "W ";

                    break;
                }
            }
            std::cout << RESET;
        }
        std::cout << RESET << std::endl;
    }
}

List<std::pair<int, int>> ReversiBoard::getPlaceableCells(const stone color) {
    return placeableCells[color];
}

int ReversiBoard::getStoneCount(const stone color) const {
    int count = 0;
    for (const std::vector<stone>&row: board) {
        for (const stone&s: row) {
            if (s == color) {
                count++;
            }
        }
    }
    return count;
}

List<std::pair<int, int>> ReversiBoard::getStoneList(const stone color) const {
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

std::map<stone, int> ReversiBoard::getStonesCount() const {
    std::map<stone, int> stonesCount = std::map<stone, int>();
    stonesCount[stone::Black] = 0;
    stonesCount[stone::White] = 0;
    stonesCount[stone::Empty] = 0;
    for (const std::vector<stone>&row: board) {
        for (const stone&s: row) {
            stonesCount[s]++;
        }
    }
    return stonesCount;
}


bool ReversiBoard::finished() {
    return
            /// 空きマスがあるかどうか
            getStoneCount(stone::Empty) == 0 ||
            // 黒が置けるかどうか
            (getPlaceableCells(stone::Black).size() == 0 &&
             /// 白が置けるかどうか
             getPlaceableCells(stone::White).size() == 0);
}
