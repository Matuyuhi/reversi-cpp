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

#include "MessageHandler.h"
#include "NetworkEntity.h"
#include "Sender.h"

#include "../../riversi/public/ReversiBoard.h"


namespace winsoc
{
    class ReversiServer : public NetworkEntity
    {

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
        enum ClientState
        {
            Idle,
            InRiversi,
            None
        };

        using ClientInfo = struct _ClientInfo
        {
            SOCKET socket = 0;
            int id = -1;
            ClientState state = None;
        };

        using SessionInfo = struct _SessionInfo
        {
            int clientA = -1;
            int clientB = -1;
            int sessionId = -1;
            ReversiBoard board;
        };

        std::unordered_map<int, ClientInfo> clientSockets = std::unordered_map<int, ClientInfo>();
        std::atomic<int> nextClientId{1};
        std::mutex socketsMutex;
        std::unordered_map<int, SessionInfo> sessions = std::unordered_map<int, SessionInfo>();

        void AddClient(const ClientInfo& client)
        {
            clientSockets[client.id] = client;
        }

        void AddSession(const SessionInfo& session)
        {
            sessions[session.sessionId] = session;
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
                case MessageType::RequestUserList:
                    SendUserList(clientId);
                    break;
                case MessageType::RequestConnectToPlayClient:
                    UserPlayConnectRequest(clientId, message);
                    break;
                case MessageType::FailConnectedPlayClient:
                case MessageType::RequestGameStart:
                    UserPlayConnectResponse(clientId, message);
                    break;
                default:
                    break;
                }
            }
            CloseClientConnection(clientId);
        }

        void SendUserList(int clientId) const
        {
            if (clientSockets.contains(clientId))
            {
                ClientInfo client = clientSockets.at(clientId);
                std::vector<int> userList;
                for (std::pair<const int, ClientInfo> client : clientSockets)
                {
                    if (clientId == client.first)
                    {
                        continue;
                    }
                    userList.push_back(client.first);
                }
                Sender::SendUserList(client.socket, userList);
            }
            // todo Not found user
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

        void UserPlayConnectRequest(int clientId, Message& message) const
        {
            int requestId = 0;
            if (MessageHandler::GetSingleIntValue(message, requestId))
            {
                // todo Bad Request
                std::cout << "Bad Request\n";
                return;
            }
            if (clientSockets.contains(requestId))
            {
                ClientInfo requestClient = clientSockets.at(requestId);
                Sender::SendUserPlayRequested(requestClient.socket, clientId);
                return;
            }
        }
    
        void UserPlayConnectResponse(int clientId, Message& message)
        {
            int requestId = -1;
            if (MessageHandler::GetSingleIntValue(message, requestId))
            {
                std::cout << "Bad Request\n";
                return;
            }
            if (clientSockets.contains(requestId))
            {
                ClientInfo requestClient = clientSockets.at(requestId);
                if (message.type == MessageType::RequestGameStart)
                {
                   GameStart(clientId, requestId);
                }
                else
                {
                    Sender::FailConnectedPlayClient(requestClient.socket, clientId);
                }
                return;
            }
        }


        void GameStart(int a, int b)
        {
            const ClientInfo clientA = clientSockets.at(a);
            const ClientInfo clientB = clientSockets.at(b);
            Sender::SendGameStart(clientA.socket, b);
            Sender::SendGameStart(clientB.socket, a);
            int sessionId = static_cast<int>(sessions.size()) + 1;
            SessionInfo session{
                a,
                b,
                sessionId,
                ReversiBoard()
            };
            AddSession(session);
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
