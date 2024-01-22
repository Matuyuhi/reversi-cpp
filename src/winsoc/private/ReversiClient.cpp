﻿//
// Created by matuyuhi on 2023/12/20.
//

#include "../public/ReversiClient.h"

namespace winsoc
{
    void ReversiClient::Start() {
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

    bool ReversiClient::IsInReversi() {
        return
                state == ClientState::InReversi ||
                state == ClientState::WaitMoveRequest ||
                state == ClientState::WaitMoveResponse;
    }

    void ReversiClient::InputHandler(const std::pair<int, std::string>& input) {
        if (input.second == "end")
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

    void ReversiClient::RequestPlayInputHandler(const std::pair<int, std::string>& input) {
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

    void ReversiClient::IdleInputHandler(const std::pair<int, std::string>& input) {

        if (input.first == INPUT_ERROR_NUMBER)
        {
            Sender::SendMsg(connectSocket, input.second);
            return;
        }
        if (input.first == 0)
        {
            Sender::SendRequestUserList(connectSocket);
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
        } else {
            std::cout << "指定のユーザーが見つかりません" << '\n';
        }

    }

    void ReversiClient::InReversiInputHandler(const std::pair<int, std::string>& input) {
        if (state == ClientState::WaitMoveRequest)
        {
            if (input.first == INPUT_ERROR_NUMBER)
            {
                std::cout << "入力が不正です" << '\n';
                return;
            }

            if (inputCol == -1)
            {
                inputCol = input.first - 1;
                std::cout << "横(1~8):";
            }
            else if (inputRow == -1)
            {
                inputRow = input.first - 1;
                SendMoveRequest();
            }
            else
            {
                // 例外
                std::cout << "error: 001" << '\n';
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

    void ReversiClient::SendMoveRequest() {
        if (inputCol == -1 || inputRow == -1)
        {
            std::cout << Strings::ErrorClientMoveRequest << '\n';
            return;
        }
        Sender::SendRequestMove(connectSocket, inputCol, inputRow);
        state = ClientState::WaitMoveResponse;
    }

    void ReversiClient::ReceiveMessages() {
        char buffer[INPUT_BUFFER_SIZE];
        while (running)
        {
            const int bytesReceived = recv(connectSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0)
            {
                std::cout << "サーバーからの切断、またはエラーが発生しました。\n続行するには何かキーを押してください\n";
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
                    std::cout << "接続成功\n";
                    MessageHandler::GetSingleIntValue(message, userId);
                    std::cout << "あなたのIDは" << userId << "です。\n";
                    std::cout << "endと入力するといつでも終了できます。\n";
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

    void ReversiClient::FinishedGame(const Message& message) {
        Result result = message.result;
        std::map<stone, int> stonesCount = reversiBoard.getStonesCount();
        spacer();
        std::cout << std::format("{:<21} {}\n", "プレイヤーの石の数:", stonesCount[stone::Black]);
        std::cout << std::format("{:<21} {}\n", "COMの石の数:", stonesCount[stone::White]);
        spacer();
        std::cout << Strings::GameResultMessage(result) << '\n';
        spacer();
        state = ClientState::Idle;

        Sender::SendRequestUserList(connectSocket);
    }

    void ReversiClient::UpdateStone(const Message& message) {
        std::vector<int> response = MessageHandler::GetPlaceStone(message);
        if (response.size() != 3)
        {
            std::cout << "error: 002" << '\n';
            return;
        }
        reversiBoard.SetStone(response[1], response[2], static_cast<stone>(response[0]));
        reversiBoard.PrintBoard(color);
    }

    void ReversiClient::GetMoveResponse(const Message& message) {
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

    void ReversiClient::RequireMoveInput() {
        std::cout << Strings::YourTurn << '\n';
        state = ClientState::WaitMoveRequest;
        inputCol = -1;
        inputRow = -1;
        std::cout << "縦(1~8):";
    }

    void ReversiClient::GetPlayUserResponse(const Message& message) {
        int requestId = 0;
        if (MessageHandler::GetSingleIntValue(message, requestId))
        {
            std::cout << "Bad Request\n";
            return;
        }
        if (message.type == MessageType::FailConnectedPlayClient)
        {
            // fail
            std::cout << "始められませんでした。もう一度遊ぶ相手を選んでください。\n";
            state = ClientState::Idle;
            return;
        }
        // play
        color = (stone)requestId;
        state = ClientState::InReversi;
        reversiBoard.Init();
        reversiBoard.PrintBoard(color);
    }

    void ReversiClient::GetRequestUserPlay(const Message& message) {
        int requestedId = -1;
        MessageHandler::GetSingleIntValue(message, requestedId);
        std::cout << requestedId << "からの対戦リクエストが来ました。\n";
        std::cout << "対戦する場合は1,しない場合は0を入力してください\n入力:";
        state = ClientState::RequestedPlay;
        otherId = requestedId;
    }

    void ReversiClient::GetUserList(const Message& message) {
        MessageHandler::GetUserList(message, currentUserList);
        if (currentUserList.empty())
        {
            std::cout << "ほかにユーザーがいません。\n";
        }
        else
        {
            std::cout << "ユーザーリスト:\n";
            for (int user : currentUserList)
            {
                std::cout << "ClientId: " << user << '\n';
            }
        }
        std::cout << "更新するには0,遊ぶ場合は相手のClientIdを入力してください\n入力:" << '\n';
    }

    std::string ReversiClient::GetUserInput(const std::string &prompt) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }

    SOCKET ReversiClient::SetupConnection() const {
        // ユーザーからの入力を受け取る関数

        // ユーザーからIPアドレスとポート番号を受け取る
        std::string ipAddress = GetUserInput("IPアドレスを入力してください（Enterでデフォルト：" + DEFAULT_IP_ADDRESS + "）: ");
        std::string portInput = GetUserInput("ポート番号を入力してください（Enterでデフォルト：" + std::to_string(DEFAULT_PORT_NUMBER) + "）: ");

        // 入力が空の場合はデフォルト値を使用
        if (ipAddress.empty()) {
            ipAddress = DEFAULT_IP_ADDRESS;
        }
        int portNum = portInput.empty() ? DEFAULT_PORT_NUMBER : std::stoi(portInput);

        const SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (connectSocket == INVALID_SOCKET)
        {
            std::cout << "ソケットオープンエラー\n";
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
            std::cout << "エラー\n";
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
            std::cout << "接続できませんでした\n";
            closesocket(connectSocket);
            WSACleanup();
            return -1;
        }
        std::cout << "接続成功\n";
        return connectSocket;
    }

} // client
