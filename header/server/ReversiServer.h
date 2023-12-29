//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSISERVER_H
#define REVERSISERVER_H
#include <iostream>
#include <map>
#include <thread>
#include "NetworkEntity.h"
#include "../Input.h"


namespace server {
    class ReversiServer : public NetworkEntity {
    private:
        std::map<int, SOCKET> clientSockets;
        std::atomic<int> nextClientId{1};
    

    public:
        void Start() {
            InitializeWinsock();

            SOCKET listenSocket = SetupListeningSocket(portNum);

            while (true) {
                SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
                if (clientSocket == INVALID_SOCKET) {
                    std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
                    continue;
                }

                int clientId = nextClientId++;
                clientSockets[clientId] = clientSocket;

                std::thread(&HandleClient, this, clientId, clientSocket).detach();
            }
        }

    private:
        void HandleClient(int clientId, SOCKET clientSocket) {
            char buffer[1024];

            while (true) {
                int bytesReceived = recv(clientSocket, buffer, 1024, 0);
                if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
                    closesocket(clientSocket);
                    clientSockets.erase(clientId);
                    break;
                }

                std::string msgReceived(buffer, bytesReceived);
                std::string response = "Client " + std::to_string(clientId) + ": " + msgReceived;
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }

        SOCKET SetupListeningSocket(int port) {
            SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
            serverAddr.sin_port = htons(port);
            serverAddr.sin_addr.s_addr = INADDR_ANY;

            if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
                std::cout << "bindのエラー\n";
                closesocket(listenSocket);
                WSACleanup();
                return -1;
            } else {
                std::cout << "bind成功\n";
            }

            if (listen(listenSocket, 0) == SOCKET_ERROR){
                std::cout << "listen error.\n";
                closesocket(listenSocket);
                WSACleanup();
                return -1;
            }
            else {
                std::cout << "listen成功\n";
            }

            return listenSocket;
        }
    };
} // server

#endif //REVERSISERVER_H
