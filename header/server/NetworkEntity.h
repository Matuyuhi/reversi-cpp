//
// Created by matuyuhi on 2023/12/20.
//

#ifndef NETWORKENTITY_H
#define NETWORKENTITY_H
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include "../Input.h"

#pragma comment(lib, "Ws2_32.lib")

namespace server
{
    class NetworkEntity
    {
    public:
        virtual ~NetworkEntity() = default;
        virtual void Start() = 0; // サーバーまたはクライアントを開始するための純粋仮想関数

    protected:
        int portNum = 9122;
        WSADATA wsaData;

        void InitializeWinsock()
        {
            int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (result != 0)
            {
                std::cerr << "WSAStartup failed: " << result << std::endl;
                exit(1);
            }


            std::cout << "受け付けるポートを指定してください" << std::endl;
            std::pair<int, std::string> input = Input::getInputNumbers();
            if (input.first == INPUT_ERROR_NUMBER)
            {
                std::cout << "エラーが発生しました。" << std::endl;
                std::cout << "デフォルトのポート: " << portNum << "を使用します" << std::endl;
                return;
            }
            portNum = input.first;
        }

        void CleanupWinsock()
        {
            WSACleanup();
        }
    };
} // server

#endif //NETWORKENTITY_H
