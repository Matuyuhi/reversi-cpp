//
// Created by matuyuhi on 2023/12/20.
//

#include "../public/ReversiServer.h"

namespace winsoc
{
    SOCKET ReversiServer::SetupListeningSocket() const
    {
        
        /// ポート番号の指定 ///
        const std::string portInput = GetUserInput(Strings::ClientInputPort(std::to_string(DEFAULT_PORT_NUMBER)));
        const int portNum = portInput.empty() ? DEFAULT_PORT_NUMBER : std::stoi(portInput);

        const SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET)
        {
            std::cerr << Strings::SocketOpenError << ": " << WSAGetLastError() << '\n';
            WSACleanup();
            return -1;
        }
        sockaddr_in serverAddr;
        ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(portNum);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            std::cerr << Strings::Error(WSAGetLastError()) << '\n';
            if (errorCode == WSAEADDRINUSE)
            {
                std::cerr << Strings::AlreadyOpenedPortError << "\n";
            }
            closesocket(listenSocket);
            WSACleanup();
            return -1;
        }
        std::cout << "bind成功\n";

        if (listen(listenSocket, 0) == SOCKET_ERROR)
        {
            std::cerr << Strings::Error(WSAGetLastError()) << '\n';
            closesocket(listenSocket);
            WSACleanup();
            return -1;
        }
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
        {
            std::cerr << Strings::GetHostNameError << ": " << WSAGetLastError() << '\n';
        }
        else
        {
            struct hostent *host = gethostbyname(hostname);
            if (host != nullptr)
            {
                std::cout << Strings::HostName << ": " << host->h_name << '\n';
                std::cout << Strings::IpAddr << ": \n";
                for (int i = 0; host->h_addr_list[i] != 0; ++i)
                {
                    struct in_addr addr;
                    memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
                    std::cout << inet_ntoa(addr) << "\n";
                }
            }
        }

        std::cout << Strings::ListenPort << ": " << ntohs(serverAddr.sin_port) << '\n';

        std::cout << Strings::ServerSuccessListen <<"\n";

        return listenSocket;
    }

    void ReversiServer::CloseClientConnection(int clientId) {
        ClientInfo* client = GetClient(clientId);
        if (client == nullptr) return;
        client->state = Disconnect;
        for (auto session : sessions)
        {
            if (session.second->IsClient(clientId))
            {
                if (session.second->clients[0]->state == Disconnect && session.second->clients[1]->state == Disconnect)
                {
                    delete session.second;
                    sessions.erase(session.first);
                }
                else
                {
                    session.second->ErrorDisconnect(clientId);
                }
            }
        }
        std::lock_guard<std::mutex> lock(socketsMutex);
        if (clientSockets.contains(clientId))
        {
            closesocket(clientSockets[clientId]->socket);
            clientSockets[clientId]->state = Disconnect;
            std::cout << Strings::DisconnectedClient(clientId) << '\n';
            DeleteClient(clientId);
        }
    }

    void ReversiServer::GameStart(int a, int b) {
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
        clientA->state = InReversi;
        clientB->state = InReversi;
        session->Initialized();
        AddSession(session);
    }

    void ReversiServer::UserPlayConnectResponse(int clientId, Message &message) {
        int requestId = -1;
        if (MessageHandler::GetSingleIntValue(message, requestId))
        {
            std::cout << Strings::BadResponse << "\n";
            return;
        }
        if (clientSockets.contains(requestId))
        {
            // 対戦のrequest元
            ClientInfo* requestClient = clientSockets.at(requestId);
            if (message.type == MessageType::RequestGameStart && requestClient->state == Idle && clientSockets.at(clientId)->state == Idle)
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

    void ReversiServer::AddClient(ClientInfo *client) {
        clientSockets[client->id] = client;
    }

    void ReversiServer::AddSession(SessionInfo *session) {
        sessions[session->sessionId] = session;
    }

    void ReversiServer::DeleteClient(int clientId) {
        // クライアントIDに基づいてエントリを削除
        clientSockets.erase(clientId);
    }

    ClientInfo *ReversiServer::GetClient(int clientId) {
        const auto it = clientSockets.find(clientId);
        if (it != clientSockets.end() && it->second->id == clientId)
        {
            return (it->second);
        }
        return nullptr;
    }

    void ReversiServer::HandleClient(int clientId) {
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
                    RequestReversiMessage(clientId, message);
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
                case MessageType::RequestMove:
                    OnMove(clientId, message);
                    break;
                default:
                    break;
            }
            if (message.type == MessageType::Disconnected)
            {
                break;
            }
        }
        CloseClientConnection(clientId);
    }

    void ReversiServer::OnMove(int clientId, Message &message) const
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

    void ReversiServer::SendUserList(int clientId) const {
        if (clientSockets.contains(clientId))
        {
            ClientInfo* client = clientSockets.at(clientId);
            std::vector<int> userList;
            for (std::pair<const int, ClientInfo*> clt : clientSockets)
            {
                if (clientId == clt.first)
                {
                    continue;
                }
                userList.push_back(clt.first);
            }
            Sender::SendUserList(client->socket, userList);
        }
        // todo Not found user
    }

    void ReversiServer::SendAllClient(int ownerId, const std::string& message) const {
        for (const std::pair<const int, ClientInfo*> clt : clientSockets)
        {
            if (clt.second->id == ownerId)
            {
                continue;
            }
            Sender::SendMsg(clt.second->socket, Strings::SendUserMessageFormat(ownerId, message));
        }
    }

    void ReversiServer::RequestReversiMessage(int clientId, Message &message) const {
        int sendId = -1;
        for (const auto &item: sessions) {
            if (item.second->IsClient(clientId)) {
                sendId = item.second->clients[0]->id == clientId ? item.second->clients[1]->id : item.second->clients[0]->id;
                break;
            }
        }
        if (clientSockets.contains(sendId))
        {
            ClientInfo* sendClient = clientSockets.at(sendId);
            Sender::SendMsg(sendClient->socket, message.payload);
        }
    }

    void ReversiServer::UserPlayConnectRequest(int clientId, Message &message) const {
        int requestId = 0;
        if (MessageHandler::GetSingleIntValue(message, requestId))
        {
            std::cout << Strings::BadResponse << "\n";
            return;
        }
        if (clientSockets.contains(requestId))
        {
            ClientInfo* requestClient = clientSockets.at(requestId);
            if (requestClient->state != Idle)
            {
                Sender::SendMsg(clientSockets.at(clientId)->socket, Strings::FailUserConnect(requestId));
                return;
            }
            Sender::SendUserPlayRequested(requestClient->socket, clientId);
        }
    }

    void ReversiServer::Start() {
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
                std::cerr << Strings::AcceptFailed << ": " << WSAGetLastError() << '\n';
                continue;
            }

            int clientId = -1;
            for(int i = 1; i <= maxConnections; ++i)
            {
                if (!clientSockets.contains(i))
                {
                    clientId = i;
                    break;
                }
            }
            if (clientId == -1)
            {
                Sender::SendMsg(clientSocket, Strings::MaxClientCountError);
                closesocket(clientSocket);
                continue;
            }
            ClientInfo* client = new ClientInfo{clientSocket, clientId, Idle};
            AddClient(client);

            std::cout << Strings::AddedNewClient << ": " << clientId << '\n';

            /// クライアントごとにスレッドを立てる
            std::thread([this, clientId]()
                        {
                            this->HandleClient(clientId);
                        }).detach();
        }
    }

    ReversiServer::~ReversiServer() {
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
} // winsoc
