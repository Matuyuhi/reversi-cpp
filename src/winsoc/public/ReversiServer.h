//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSISERVER_H
#define REVERSISERVER_H
#include <map>
#include <iostream>
#include <thread>
#include <atomic>
#include <list>
#include <mutex>
#include <sstream>
#include <unordered_map>

#include "NetworkEntity.h"
#include "Sender.h"


namespace winsoc
{
    class ReversiServer : public NetworkEntity
    {
    private:
        enum ClientState
        {
            Idle,
            InRiversi,
            None
        };

        using ClientInfo = struct ClientInfo
        {
            SOCKET socket = 0;
            int id = -1;
            ClientState state = None;
        };

        std::unordered_map<int, ClientInfo> clientSockets;
        std::atomic<int> nextClientId{1};
        std::mutex socketsMutex;

        void AddClient(const ClientInfo& client)
        {
            clientSockets[client.id] = client;
        }

        // クライアントを削除する関数
        void DeleteClient(int clientId)
        {
            // クライアントIDに基づいてエントリを削除
            clientSockets.erase(clientId);
        }

        ClientInfo* GetClient(int clientId)
        {
            const auto it = clientSockets.find(clientId);
            if (it != clientSockets.end() && it->second.id == clientId)
            {
                return &(it->second);
            }
            return nullptr;
        }

    public:
        void Start() override
        {
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
                AddClient(ClientInfo{clientSocket, clientId, Idle});

                std::cout << "new Client: " << clientId << '\n';

                /// クライアントごとにスレッドを立てる
                std::thread([this, clientId]()
                {
                    this->HandleClient(clientId);
                }).detach();
            }
        }

    private:
        void HandleClient(int clientId)
        {
            ClientInfo* client = GetClient(clientId);
            char buffer[INPUT_BUFFER_SIZE];
            Sender::SendConnected(client->socket, clientId);
            while (true)
            {
                client = GetClient(clientId);
                std::cout << "受信待ち" << '\n';
                const int bytesReceived = recv(client->socket, buffer, INPUT_BUFFER_SIZE, 0);
                if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
                {
                    std::cout << "エラーが発生しました。" << '\n';
                    closesocket(client->socket);
                    DeleteClient(clientId);
                    break;
                }

                std::string msgReceived(buffer, bytesReceived);
                Message message = Sender::Deserialize(msgReceived);
                message.CoutMessage();

                switch (message.type)
                {
                case MessageType::RequestMessage:
                    SendAllClient(clientId, message.payload);
                    break;
                default:
                    break;
                }
            }
            CloseClientConnection(clientId);
        }

        void SendAllClient(int ownerId, std::string message) const
        {
            for (const std::pair<const int, ClientInfo> clt : clientSockets)
            {
                if (clt.second.id == ownerId)
                {
                    continue;
                }
                Sender::SendMsg(clt.second.socket, "client" + std::to_string(ownerId) + ":" + message);
            }
        }


        SOCKET SetupListeningSocket() const
        {
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
            if (clientSockets.contains(clientId))
            {
                closesocket(clientSockets[clientId].socket);
                clientSockets.erase(clientId);
                std::cout << "クライアント " << clientId << " が切断されました。\n";
            }
        }
    };
} // winsoc

#endif //REVERSISERVER_H
