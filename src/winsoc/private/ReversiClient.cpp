//
// Created by matuyuhi on 2023/12/20.
//

#include "../public/ReversiClient.h"

namespace winsoc
{
    void ReversiClient::Start()
    {
        InitializeWinsock();

        std::cin.clear();
        std::cin.ignore(INPUT_BUFFER_SIZE, '\n');

        connectSocket = SetupConnection();
        if (connectSocket == -1)
        {
            return;
        }

        std::thread receiveThread(&ReversiClient::ReceiveMessages, this);
        while (running)
        {
            std::pair<int, std::string> input = Input::getInputNumbers();
            InputHandler(input);
        }
        std::cout << "終了\n";
        receiveThread.join();
        closesocket(connectSocket);
        CleanupWinsock();
    }

    bool ReversiClient::IsInReversi()
    {
        return
            state == ClientState::InReversi ||
            state == ClientState::WaitMoveRequest ||
            state == ClientState::WaitMoveResponse;
    }

    void ReversiClient::InputHandler(const std::pair<int, std::string>& input)
    {
        if (input.second == Strings::End)
        {
            std::cout << Strings::SendDisconnect << "\n";
            Sender::SendDisconnected(connectSocket);
            running = false;
            return;
        }
        if (state == ClientState::Idle)
        {
            IdleInputHandler(input);
        }
        else if (IsInReversi())
        {
            InReversiInputHandler(input);
        }
        else if (state == ClientState::RequestedPlay)
        {
            RequestPlayInputHandler(input);
        }
    }

    void ReversiClient::RequestPlayInputHandler(const std::pair<int, std::string>& input)
    {
        if (input.first == 1)
        {
            // play
            std::cout << Strings::GameStartOk << "\n";
            Sender::RequestGameStart(connectSocket, otherId);
            state = ClientState::InReversi;
        }
        else if (input.first == 0)
        {
            // fail
            std::cout << Strings::GameStartNg << "\n";
            Sender::FailConnectedPlayClient(connectSocket, otherId);
            state = ClientState::Idle;
        }
        else
        {
            std::cout << Strings::InputFormatFailed << "\n";
        }
    }

    void ReversiClient::IdleInputHandler(const std::pair<int, std::string>& input) const
    {
        if (input.first == INPUT_ERROR_NUMBER)
        {
            Sender::SendMsg(connectSocket, input.second);
            return;
        }
        // refresh userLit
        if (input.first == Strings::refreshUserListNum)
        {
            Sender::SendRequestUserList(connectSocket);
            return;
        }

        if (input.first == Strings::vsComNum)
        {
            Sender::SendGameStartWithCOM(connectSocket);
            return;
        }
        int findId = -1;
        for (int uId : currentUserList)
        {
            if (input.first == uId)
            {
                findId = uId;
                break;
            }
        }
        if (findId != -1)
        {
            std::cout << Strings::SendGameRequest(findId) << "\n";
            Sender::SendRequestPlayClient(connectSocket, findId);
            return;
        }
        else
        {
            std::cout << Strings::NotFoundUser << '\n';
        }
    }

    void ReversiClient::InReversiInputHandler(const std::pair<int, std::string>& input)
    {
        if (state == ClientState::WaitMoveRequest)
        {
            if (input.first == INPUT_ERROR_NUMBER)
            {
                std::cout << Strings::InputFormatFailed << '\n';
                return;
            }

            if (inputCol == -1)
            {
                inputCol = input.first - 1;
                std::cout << Strings::InputRow;
            }
            else if (inputRow == -1)
            {
                inputRow = input.first - 1;
                SendMoveRequest();
            }
            else
            {
                // 例外
                std::cout << Strings::Error(1) << '\n';
            }
        }
        else if (state == ClientState::WaitMoveResponse)
        {
            std::cout << Strings::WaitServer << '\n';
        }
        else
        {
            Sender::SendInReversiMsg(connectSocket, input.second);
        }
    }

    void ReversiClient::SendMoveRequest()
    {
        if (inputCol == -1 || inputRow == -1)
        {
            std::cout << Strings::ErrorClientMoveRequest << '\n';
            return;
        }
        Sender::SendRequestMove(connectSocket, inputCol, inputRow);
        state = ClientState::WaitMoveResponse;
    }

    void ReversiClient::ReceiveMessages()
    {
        char buffer[INPUT_BUFFER_SIZE];
        while (running)
        {
            const int bytesReceived = recv(connectSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0)
            {
                std::cout << Strings::ServerConnectError << "\n";
                running = false;
                break;
            }
            buffer[bytesReceived] = '\0';
            const Message message = Sender::Deserialize(buffer);
            // message.CoutMessage();
            switch (message.type)
            {
            case MessageType::RequestMessage:
                // todo
                std::cout << message.payload << "\n";
                break;
            case MessageType::Connected:
                // todo
                std::cout << Strings::SuccessConnect << "\n";
                MessageHandler::GetSingleIntValue(message, userId);
                std::cout << Strings::ServerConnectedMessage(userId) << '\n';
                Sender::SendRequestUserList(connectSocket);
                break;
            case MessageType::UserList:
                GetUserList(message);
                break;
            case MessageType::UserPlayRequested:
                GetRequestUserPlay(message);
                break;
            case MessageType::FailConnectedPlayClient:
            case MessageType::GameStart:
                GetPlayUserResponse(message);
                break;
            case MessageType::ResponseMove:
                GetMoveResponse(message);
                break;
            case MessageType::PlaceStone:
                UpdateStone(message);
                break;
            case MessageType::WaitMove:
                RequireMoveInput();
                break;
            case MessageType::GameEnd:
                FinishedGame(message);
                break;
            default:
                break;
            }
        }
    }

    void ReversiClient::FinishedGame(const Message& message)
    {
        Result result = message.result;
        std::map<stone, int> stonesCount = reversiBoard.getStonesCount();
        spacer();
        std::cout << Strings::StoneCount(true, stonesCount[color]);
        std::cout << Strings::StoneCount(false, stonesCount[!color]);
        spacer();
        std::cout << Strings::GameResultMessage(result) << '\n';
        spacer();
        state = ClientState::Idle;

        Sender::SendRequestUserList(connectSocket);
    }

    void ReversiClient::UpdateStone(const Message& message)
    {
        std::vector<int> response = MessageHandler::GetPlaceStone(message);
        if (response.size() != 3)
        {
            std::cout << Strings::Error(2) << '\n';
            return;
        }
        reversiBoard.SetStone(response[1], response[2], static_cast<stone>(response[0]));
        reversiBoard.PrintBoard(color);
    }

    void ReversiClient::GetMoveResponse(const Message& message)
    {
        std::cout << message.payload << '\n';
        if (message.result == Result::Success)
        {
            state = ClientState::InReversi;
        }
        else if (message.result == Result::Error)
        {
            //もういちどにゅうりょく
            RequireMoveInput();
        }
    }

    void ReversiClient::RequireMoveInput()
    {
        std::cout << Strings::YourTurn << '\n';
        state = ClientState::WaitMoveRequest;
        inputCol = -1;
        inputRow = -1;
        std::cout << Strings::InputCol;
    }

    void ReversiClient::GetPlayUserResponse(const Message& message)
    {
        int requestId = 0;
        if (MessageHandler::GetSingleIntValue(message, requestId))
        {
            std::cout << Strings::BadResponse << "\n";
            return;
        }
        if (message.type == MessageType::FailConnectedPlayClient)
        {
            // fail
            std::cout << Strings::StartFailedPleaseReselect << "\n";
            state = ClientState::Idle;
            return;
        }
        // play
        color = static_cast<stone>(requestId);
        state = ClientState::InReversi;
        reversiBoard.Init();
        reversiBoard.PrintBoard(color);
    }

    void ReversiClient::GetRequestUserPlay(const Message& message)
    {
        int requestedId = -1;
        MessageHandler::GetSingleIntValue(message, requestedId);
        std::cout << Strings::GameRequested(requestedId);
        state = ClientState::RequestedPlay;
        otherId = requestedId;
    }

    void ReversiClient::GetUserList(const Message& message)
    {
        MessageHandler::GetUserList(message, currentUserList);
        if (currentUserList.empty())
        {
            std::cout << Strings::NotFoundUser << "\n";
        }
        else
        {
            std::cout << Strings::UserList << ":\n";
            for (int user : currentUserList)
            {
                std::cout << "ClientId: " << user << '\n';
            }
        }
        std::cout << Strings::NavRefreshUserList << "\n入力:" << '\n';
    }

    SOCKET ReversiClient::SetupConnection() const
    {
        // ユーザーからの入力を受け取る関数

        // ユーザーからIPアドレスとポート番号を受け取る
        std::string ipAddress = GetUserInput(Strings::ClientInputIP(DEFAULT_IP_ADDRESS));
        std::string portInput = GetUserInput(Strings::ClientInputPort(std::to_string(DEFAULT_PORT_NUMBER)));

        // 入力が空の場合はデフォルト値を使用
        if (ipAddress.empty())
        {
            ipAddress = DEFAULT_IP_ADDRESS;
        }
        int portNum = portInput.empty() ? DEFAULT_PORT_NUMBER : std::stoi(portInput);

        const SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (connectSocket == INVALID_SOCKET)
        {
            std::cout << Strings::SocketOpenError << "\n";
            WSACleanup();
            return -1;
        }

        const HOSTENT* lpHost = gethostbyname(ipAddress.c_str());
        if (lpHost == nullptr)
        {
            unsigned int addr = inet_addr(ipAddress.c_str());
            lpHost = gethostbyaddr(reinterpret_cast<char*>(&addr), 4, AF_INET);
        }
        if (lpHost == nullptr)
        {
            std::cout << Strings::Error(3) << "\n";
            closesocket(connectSocket);
            WSACleanup();
            return -1;
        }

        SOCKADDR_IN saddr;
        ZeroMemory(&saddr, sizeof(SOCKADDR_IN));
        saddr.sin_family = lpHost->h_addrtype;
        saddr.sin_port = htons(portNum);
        saddr.sin_addr.s_addr = *reinterpret_cast<u_long*>(lpHost->h_addr);
        if (connect(connectSocket, reinterpret_cast<SOCKADDR*>(&saddr), sizeof(saddr)) == SOCKET_ERROR)
        {
            std::cout << Strings::NotConnection << "\n";
            closesocket(connectSocket);
            WSACleanup();
            return -1;
        }
        std::cout << Strings::SuccessConnect << "\n";
        return connectSocket;
    }
} // client
