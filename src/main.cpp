//
// Created by matuyuhi on 2023/11/03.
//
#include "../header/ReversiGame.h"

int main() {
    ReversiGame* game = new ReversiGame();
    while (true) {
        const ReversiGame::FinishedState res = game->Start();
        //　例外
        if (res == ReversiGame::FinishedState::Error) {
            std::cout << "エラーが発生しました" << std::endl;
            break;
        }
        // 終わり
        if (res == ReversiGame::FinishedState::Quit) {
            std::cout << "終了します" << std::endl;
            break;
        }
        // 再スタート
        game->Clear();
    }
    delete game;

    return 0;
}
