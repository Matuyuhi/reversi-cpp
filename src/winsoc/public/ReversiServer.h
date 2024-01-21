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

#include "ReversiSessionManager.h"
#include "Strings.h"


namespace winsoc
{
    class ReversiServer : public NetworkEntity
    {
    private:
        enum ClientState
        {
            Idle,
            InReversi,
            Disconnect,
            None
        };

        using ClientInfo = struct _ClientInfo
        {
            SOCKET socket;
            int id;
            ClientState state;
        };
        inline static ClientInfo ClientNone = ClientInfo{0, -1, None};

        using SessionInfo = struct _SessionInfo
        {
            ClientInfo* clients[2];
            int currentTurn = -1;
            int sessionId = -1;
            ReversiSessionManager board;

            _SessionInfo()
            {
                clients[0] = &ClientNone;
                clients[1] = &ClientNone;
                sessionId = -1;
                board = ReversiSessionManager();
            }

            _SessionInfo(ClientInfo* clientA, ClientInfo* clientB, int sessionId)
            {
                clients[0] = clientA;
                clients[1] = clientB;
                this->sessionId = sessionId;
                board = ReversiSessionManager();
            }

            ~_SessionInfo()
            {
                // delete[] clients;
            }

            void Initialized()
            {
                board = ReversiSessionManager();
                board.Initialized();
                currentTurn = 0;
                SendToggleTurn();
            }

            void OnMove(int clientId, int row, int col)
            {
                const ClientInfo* client = clients[currentTurn];
                const ClientInfo* other = clients[currentTurn == 0 ? 1 : 0];
                const stone stone = ClientStone(currentTurn);
                if (client->id != clientId && other->id == clientId)
                {
                    /// 例外
                    Sender::SendMsg(client->socket, Strings::NotYourTurn);
                    return;
                }
                if (!board.IsCanPlaceStone(row, col, stone))
                {
                    Sender::ResponseMove(client->socket, Strings::PlaceStoneResult(col, row, true, Result::Error),
                                         Result::Error);
                    return;
                }
                board.SetStone(row, col, stone);
                Sender::ResponseMove(client->socket, Strings::PlaceStoneResult(col, row, true, Result::Success),
                                     Result::Success);
                Sender::SendPlaceStone(client->socket, static_cast<int>(stone), row, col);
                Sender::ResponseMove(other->socket, Strings::PlaceStoneResult(col, row, false, Result::Success),
                                     Result::Success);
                Sender::SendPlaceStone(other->socket, static_cast<int>(stone), row, col);
                SendToggleTurn();
            }

            bool IsClient(int clientId)
            {
                return clients[0]->id == clientId || clients[1]->id == clientId;
            }

            static stone ClientStone(int num)
            {
                return num == 0 ? stone::Black : stone::White;
            }

            bool operator==(const _SessionInfo& other) const
            {
                return sessionId == other.sessionId;
            }

        private:
            void SendToggleTurn()
            {
                if (board.finished())
                {
                    SendFinished();
                    return;
                }
                Sender::SendMsg(clients[currentTurn]->socket, Strings::OtherPlayerTurn);
                // ターンの切り替え
                currentTurn = currentTurn == 0 ? 1 : 0;
                Sender::SendWaitMove(clients[currentTurn]->socket);
            }

            void SendFinished()
            {
                int index = 0;
                int counts[] = {
                    board.getStoneCount(ClientStone(0)),
                    board.getStoneCount(ClientStone(1))
                };
                for (auto client : clients)
                {
                    if (!board.getStoneCount(stone::Empty))
                    {
                        // 空きマスがない場合
                        Sender::SendMsg(client->socket, "ゲームを終了します");
                    }
                    else if (board.getPlaceableCells(stone::Black).size() == 0 || board.getPlaceableCells(stone::White).size() == 0)
                    {
                        // 空きマスがある場合
                        Sender::SendMsg(client->socket, "おけるマスがないため、ゲームを終了します");
                    }

                    if (counts[index] > counts[!index])
                    {
                        Sender::SendGameEnd(client->socket, "あなたの勝ちです", Result::GameEndWin);
                    }
                    else if (counts[index] < counts[!index])
                    {
                        Sender::SendGameEnd(client->socket,"あなたの勝ちです", Result::GameEndLose);
                    }
                    else
                    {
                        Sender::SendGameEnd(client->socket, "あなたの勝ちです", Result::GameEndDraw);
                    }
                    index++;
                }
               
            }
        };

        std::unordered_map<int, ClientInfo*> clientSockets;
        std::atomic<int> nextClientId{1};
        std::mutex socketsMutex;
        std::unordered_map<int, SessionInfo*> sessions;

        int portNum = 9122;

        void AddClient(ClientInfo* client)
        {
            clientSockets[client->id] = client;
        }

        void AddSession(SessionInfo* session)
        {
            sessions[session->sessionId] = session;
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
            if (it != clientSockets.end() && it->second->id == clientId)
            {
                return (it->second);
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
//                std::cout << "受信待ち" << '\n';
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
                case MessageType::RequestInReversiMsg:

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
                case MessageType::RequestMove:
                    OnMove(clientId, message);
                    break;
                default:
                    break;
                }
            }
            CloseClientConnection(clientId);
        }

        void OnMove(int clientId, Message& message)
        {
            SessionInfo* session = nullptr;
            for (auto value : sessions)
            {
                if (value.second->IsClient(clientId))
                {
                    session = value.second;
                    break;
                }
            }
            if (session == nullptr || session->sessionId == SessionInfo().sessionId)
            {
                Sender::SendMsg(clientSockets.at(clientId)->socket, Strings::NotInSession);
                return;
            }
            std::pair<int, int> list = MessageHandler::GetMoveInfo(message);
            int col = list.first;
            int row = list.second;
            session->OnMove(clientId, row, col);
        }

        void SendUserList(int clientId) const
        {
            if (clientSockets.contains(clientId))
            {
                ClientInfo* client = clientSockets.at(clientId);
                std::vector<int> userList;
                for (std::pair<const int, ClientInfo*> client : clientSockets)
                {
                    if (clientId == client.first)
                    {
                        continue;
                    }
                    userList.push_back(client.first);
                }
                Sender::SendUserList(client->socket, userList);
            }
            // todo Not found user
        }

        void SendAllClient(int ownerId, std::string message) const
        {
            for (const std::pair<const int, ClientInfo*> clt : clientSockets)
            {
                if (clt.second->id == ownerId)
                {
                    continue;
                }
                Sender::SendMsg(clt.second->socket, "client" + std::to_string(ownerId) + ":" + message);
            }
        }

        void SendClientToId(int ownerId, int sendId, std::string message) const
        {
            for (const std::pair<const int, ClientInfo*> clt : clientSockets)
            {
                if (clt.second->id == sendId)
                {
                    Sender::SendMsg(clt.second->socket, "client" + std::to_string(ownerId) + ":" + message);
                }
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
                ClientInfo* requestClient = clientSockets.at(requestId);
                if (requestClient->state != Idle)
                {
                    Sender::SendMsg(clientSockets.at(requestId)->socket, Strings::FailUserConnect(requestId));
                    return;
                }
                Sender::SendUserPlayRequested(requestClient->socket, clientId);
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
                // 対戦のrequest元
                ClientInfo* requestClient = clientSockets.at(requestId);
                if (message.type == MessageType::RequestGameStart)
                {
                    Sender::SendMsg(requestClient->socket, Strings::SuccessStartGame(clientId));
                    GameStart(clientId, requestId);
                }
                else
                {
                    Sender::SendMsg(requestClient->socket, Strings::FailStartGame(clientId));
                    Sender::FailConnectedPlayClient(requestClient->socket, clientId);
                }
            }
        }


        void GameStart(int a, int b)
        {
            ClientInfo* clientA = clientSockets.at(a);
            ClientInfo* clientB = clientSockets.at(b);

            Sender::SendGameStart(clientA->socket, SessionInfo::ClientStone(0));
            Sender::SendMsg(clientA->socket, Strings::CallClientStone(SessionInfo::ClientStone(0)));

            Sender::SendGameStart(clientB->socket, SessionInfo::ClientStone(1));
            Sender::SendMsg(clientB->socket, Strings::CallClientStone(SessionInfo::ClientStone(1)));
            const int sessionId = static_cast<int>(sessions.size()) + 1;
            auto session = new SessionInfo{
                clientA,
                clientB,
                sessionId
            };
            session->Initialized();
            AddSession(session);
        }

        void SendUpdatedTurn(int sessionId)
        {
            SessionInfo* session = sessions.at(sessionId);
        }

        SOCKET SetupListeningSocket()
        {
            /// ポート番号の指定 ///

            std::cout << "ポートを指定してください" << '\n';
            std::pair<int, std::string> input = Input::getInputNumbers();
            if (input.first == INPUT_ERROR_NUMBER) /// 入力が不正ならdefaultの番号を使う
            {
                std::cout << "エラーが発生しました" << '\n';
                std::cout << "デフォルトのポート: " << portNum << "を使用します" << '\n';
            }
            /// ポート番号の更新
            portNum = input.first;

            const SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (listenSocket == INVALID_SOCKET)
            {
                std::cerr << "ソケットオープンエラー: " << WSAGetLastError() << '\n';
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
                int errorCode = WSAGetLastError();
                std::cerr << "bindのエラー: " << errorCode << '\n';
                if (errorCode == WSAEADDRINUSE)
                {
                    std::cerr << "ポートが既に使用されています。\n";
                }
                closesocket(listenSocket);
                WSACleanup();
                return -1;
            }
            std::cout << "bind成功\n";

            if (listen(listenSocket, 0) == SOCKET_ERROR)
            {
                std::cerr << "listen error: " << WSAGetLastError() << '\n';
                closesocket(listenSocket);
                WSACleanup();
                return -1;
            }
            char hostname[256];
            if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
            {
                std::cerr << "gethostname failed with error: " << WSAGetLastError() << '\n';
            }
            else
            {
                struct hostent *host = gethostbyname(hostname);
                if (host != nullptr)
                {
                    std::cout << "Host name: " << host->h_name << '\n';
                    std::cout << "IP Address(es): ";
                    for (int i = 0; host->h_addr_list[i] != 0; ++i)
                    {
                        struct in_addr addr;
                        memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
                        std::cout << inet_ntoa(addr) << "\n";
                    }
                }
            }

            std::cout << "Listening on port: " << ntohs(serverAddr.sin_port) << '\n';

            std::cout << "listen成功\nクライアントで";

            return listenSocket;
        }

        void CloseClientConnection(int clientId)
        {
            std::lock_guard<std::mutex> lock(socketsMutex);
            if (clientSockets.contains(clientId))
            {
                closesocket(clientSockets[clientId]->socket);
                clientSockets[clientId]->state = Disconnect;
                std::cout << "クライアント " << clientId << " が切断されました。\n";
            }
        }

    public:
        ~ReversiServer() override
        {
            for (auto client : clientSockets)
            {
                closesocket(client.second->socket);
                delete client.second;
            }
            WSACleanup();
            for (auto session : sessions)
            {
                delete session.second;
            }
        }

        void Start() override
        {
            InitializeWinsock();

            SOCKET listenSocket = SetupListeningSocket();
            if (listenSocket == -1)
            {
                return;
            }

            while (true)
            {
                SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
                if (clientSocket == INVALID_SOCKET)
                {
                    std::cerr << "Accept failed: " << WSAGetLastError() << '\n';
                    continue;
                }

                int clientId = nextClientId++;
                auto client = new ClientInfo{clientSocket, clientId, Idle};
                AddClient(client);

                std::cout << "新しいClientが追加されました: " << clientId << '\n';

                /// クライアントごとにスレッドを立てる
                std::thread([this, clientId]()
                {
                    this->HandleClient(clientId);
                }).detach();
            }
        }
    };
} // winsoc

#endif //REVERSISERVER_H
