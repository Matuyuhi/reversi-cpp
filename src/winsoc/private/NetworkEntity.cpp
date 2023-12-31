//
// Created by matuyuhi on 2023/12/20.
//

#include "../public/NetworkEntity.h"

void winsoc::NetworkEntity::InitializeWinsock()
{
    /// Winsockの初期化
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }

    /// ポート番号の指定 ///

    std::cout << "ポートを指定してください" << '\n';
    std::pair<int, std::string> input = Input::getInputNumbers();
    if (input.first == INPUT_ERROR_NUMBER) /// 入力が不正ならdefaultの番号を使う
    {
        std::cout << "エラーが発生しました" << '\n';
        std::cout << "デフォルトのポート: " << portNum << "を使用します" << '\n';
        return;
    }
    /// ポート番号の更新
    portNum = input.first;
}

void winsoc::NetworkEntity::CleanupWinsock()
{
    WSACleanup();
}
