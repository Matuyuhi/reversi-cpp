//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSISERVER_H
#define REVERSISERVER_H
#include <map>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>

#include "../NetworkEntity.h"


namespace server {
    class ReversiServer : public NetworkEntity {
    private:
        std::map<int, SOCKET> clientSockets;
        std::atomic<int> nextClientId{1};
        std::mutex socketsMutex;


    public:
        void Start() {
            InitializeWinsock();

            SOCKET listenSocket = SetupListeningSocket();

            while (true)
            {
                SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
                if (clientSocket == INVALID_SOCKET)
                {
                    std::cerr << "Accept failed: " << WSAGetLastError() << '\n';
                    continue;
                }

                int clientId = nextClientId++;
                clientSockets[clientId] = clientSocket;

                std::cout << "new Client: " << clientId << '\n';

                /// クライアントごとにスレッドを立てる
                std::thread([this, clientId, clientSocket]()
                {
                    this->HandleClient(clientId, clientSocket);
                }).detach();
            }
        }

    private:
        void HandleClient(int clientId, SOCKET clientSocket) {
            char buffer[1024];
            const std::string msg = "new Client Accept!!! ID is " + std::to_string(clientId);
            send(clientSocket, msg.c_str(), static_cast<int>(msg.length()), 0);
            while (true)
            {
                std::cout << "受信待ち" << '\n';
                const int bytesReceived = recv(clientSocket, buffer, 1024, 0);
                if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
                {
                    closesocket(clientSocket);
                    clientSockets.erase(clientId);
                    break;
                }

                std::string msgReceived(buffer, bytesReceived);
                std::string response = "Client " + std::to_string(clientId) + ": " + msgReceived;
                for (const std::pair<int, SOCKET> socket : clientSockets)
                {
                    send(socket.second, response.c_str(), static_cast<int>(response.length()), 0);
                }

                std::cout << response << '\n';
            }
            CloseClientConnection(clientId);
        }

        SOCKET SetupListeningSocket() {
            const SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (listenSocket < 0)
            {
                std::cout << "ソケットオープンエラー\n";
                WSACleanup();
                return -1;
            }
            std::cout << "リスンソケットオープン完了。\n";
            sockaddr_in serverAddr;
            ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(portNum);
            serverAddr.sin_addr.s_addr = INADDR_ANY;

            if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
            {
                std::cout << "bindのエラー\n";
                closesocket(listenSocket);
                WSACleanup();
                return -1;
            }
            std::cout << "bind成功\n";

            if (listen(listenSocket, 0) == SOCKET_ERROR)
            {
                std::cout << "listen error.\n";
                closesocket(listenSocket);
                WSACleanup();
                return -1;
            }
            std::cout << "listen成功\n";

            return listenSocket;
        }

        void CloseClientConnection(int clientId)
        {
            std::lock_guard<std::mutex> lock(socketsMutex);
            if (clientSockets.find(clientId) != clientSockets.end())
            {
                closesocket(clientSockets[clientId]);
                clientSockets.erase(clientId);
                std::cout << "クライアント " << clientId << " が切断されました。\n";
            }
        }
    };
} // server

#endif //REVERSISERVER_H
