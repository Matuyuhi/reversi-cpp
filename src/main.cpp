//
// Created by matuyuhi on 2023/11/03.
//
#include "riversi/public/ReversiGame.h"
#include "winsoc/public/NetworkEntity.h"
#include "winsoc/public/ReversiServer.h"
#include "winsoc/public/ReversiClient.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr int SelectedServer = 0;
constexpr int SelectedClient = 1;

int main()
{
    int userSelected;
    while (true)
    {
        std::cout << "サーバーかクライアントかを選んでください" << '\n';
        std::cout << SelectedServer << ": サーバー  " << SelectedClient << ": クライアント" << '\n';
        std::pair<int, std::string> input = Input::getInputNumbers();
        if (input.first == INPUT_ERROR_NUMBER)
        {
            std::cout << "入力が不正です" << '\n';
            continue;
        }
        if (input.first != 0 && input.first != 1)
        {
            std::cout << "0か1で入力してください" << '\n';
            continue;
        }
        userSelected = input.first;
        break;
    }
    winsoc::NetworkEntity* entity;
    if (userSelected == SelectedServer)
    {
        entity = new winsoc::ReversiServer();
    }
    else
    {
        entity = new winsoc::ReversiClient();
    }
    entity->Start();
    auto game = new ReversiGame();
    while (true)
    {
        const ReversiGame::FinishedState res = game->Start();
        //　例外
        if (res == ReversiGame::FinishedState::Error)
        {
            std::cout << "エラーが発生しました" << '\n';
            break;
        }
        // 終わり
        if (res == ReversiGame::FinishedState::Quit)
        {
            std::cout << "終了します" << '\n';
            break;
        }
        // 再スタート
        game->Clear();
    }
    delete game;

    return 0;
}
