#include "../header/Stone.h"
#include "../header/ReversiBoard.h"


int main() {
    ReversiBoard board;
    if (board.isCanPlace(3, 2, stone::Black)) {
        board.placeStone(3, 2, stone::Black);
    }
    if (board.isCanPlace(2, 2, stone::White)) {
        board.placeStone(2, 2, stone::White);
    }
    if (board.isCanPlace(4, 2, stone::White)) {
        board.placeStone(4, 2, stone::White);
    }
    board.printBoard(stone::White);
    board.printBoard(stone::Black);
    return 0;
}
