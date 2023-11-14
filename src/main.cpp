//
// Created by matuyuhi on 2023/11/03.
//
#include "../header/ReversiGame.h"

int main() {
    ReversiGame* game = new ReversiGame();
    while (true) {
        const ReversiGame::FinishedState res = game->Start();
        if (res == ReversiGame::FinishedState::Restart) {
            game->Clear();
            continue;
        }
        if (res == ReversiGame::FinishedState::Quit) {
            break;
        }
        std::cout << "エラーが発生しました" << std::endl;
        break;
    }
    delete game;

    return 0;
}
