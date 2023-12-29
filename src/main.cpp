//
// Created by matuyuhi on 2023/11/03.
//
#include "../header/ReversiGame.h"
#include "../header/server/NetworkEntity.h"
#include "../header/server/ReversiServer.h"
#include "../header/server/ReversiClient.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr int SelectedServer = 0;
constexpr int SelectedClient = 1;

int main() {
    int userSelected = 0;
    while (true) {
        std::cout << "サーバーかクライアントかを選んでください" << std::endl;
        std::cout << SelectedServer << ": サーバー  " << SelectedClient << ": クライアント" << std::endl;
        std::pair<int, std::string> input = Input::getInputNumbers();
        if (input.first == INPUT_ERROR_NUMBER) {
            std::cout << "入力が不正です" << std::endl;
            continue;
        }
        if (input.first != 0 && input.first != 1) {
            std::cout << "0か1で入力してください" << std::endl;
            continue;
        }
        userSelected = input.first;
        break;
    }
    server::NetworkEntity* entity;
    if (userSelected == SelectedServer) {
        entity = new server::ReversiServer();
    }
    else {
        entity = new server::ReversiClient();
    }
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
