//
// Created by matuyuhi on 2023/11/03.
//
#include "reversi/public/ReversiGame.h"
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
        std::cout << "サーバーかクライアントかを選んでください\n入力: ";
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
    delete entity;
    return 0;
}
