//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSICLIENT_H
#define REVERSICLIENT_H
#include "../public/NetworkEntity.h"
#include <thread>

#include "MessageHandler.h"
#include "Sender.h"
#include "ReversiSessionManager.h"
#include "Strings.h"

namespace winsoc
{
    class ReversiClient : public NetworkEntity
    {
    public:
        void Start() override
        {
            InitializeWinsock();
            
            std::cin.clear();
            std::cin.ignore(INPUT_BUFFER_SIZE, '\n');
            
            connectSocket = SetupConnection("127.0.0.1");

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

    private:
        enum class ClientState
        {
            InReversi,
            WaitMoveRequest,
            WaitMoveResponse,
            Idle,
            RequestedPlay,
        };
        SOCKET connectSocket = 0;
        ClientState state = ClientState::Idle;
        std::atomic<bool> running{true};
        int userId = -1;
        int otherId = -1;
        std::vector<int> currentUserList;
        ReversiSessionManager reversiBoard = ReversiSessionManager();

        // 0~7
        int inputCol = -1;
        // 0~7
        int inputRow = -1;

        bool IsInRiversi()
        {
            return
                state == ClientState::InReversi ||
                state == ClientState::WaitMoveRequest ||
                state == ClientState::WaitMoveResponse;
        }

        void InputHandler(std::pair<int, std::string> input)
        {
            if (state == ClientState::Idle)
            {
                IdleInputHandler(input);
            }else if (IsInRiversi())
            {
                InReversiInputHandler(input);
            } else if (state == ClientState::RequestedPlay) {
                RequestPlayInputHandler(input);
       
            }
        }

        void RequestPlayInputHandler(std::pair<int, std::string> input)
        {
            if (input.first == 1)
            {
                // play
                std::cout << "対戦を承諾しました。\n";
                Sender::RequestGameStart(connectSocket, otherId);
                state = ClientState::InReversi;
            }
            else if (input.first == 0)
            {
                // fail
                std::cout << "対戦を拒否しました。\n";
                Sender::FailConnectedPlayClient(connectSocket, otherId);
                state = ClientState::Idle;
            }
            else
            {
                std::cout << "入力が不正です" << '\n';
            }
        }

        void IdleInputHandler(std::pair<int, std::string> input)
        {
            if (input.second == "end")
            {
                running = false;
                return;
            }

            if (input.first == INPUT_ERROR_NUMBER)
            {
                std::cout << "入力が不正です" << '\n';
                return;
            }
            if (input.first == 0)
            {
                Sender::SendRequestUserList(connectSocket);
                return;
            }
            int findId = -1;
            for (int userId : currentUserList)
            {
                if (input.first == userId)
                {
                    findId = userId;
                    break;
                }
            }
            if (findId != -1)
            {
                std::cout << findId << "に対して対戦リクエストを送ります。\n";
                Sender::SendRequestPlayClient(connectSocket, findId);
                return;
            }
            // std::cout << "入力が不正です" << '\n';
            Sender::SendMsg(connectSocket, input.second);
        }

        void InReversiInputHandler(std::pair<int, std::string> input)
        {
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
                } else if (inputRow == -1)
                {
                    inputRow = input.first - 1;
                    SendMoveRequest();
                } else
                {
                    // 例外
                    std::cout << "error: 001" << '\n';
                }
            } else if (state == ClientState::WaitMoveResponse)
            {
                std::cout << Strings::WaitServer << '\n';
            } else
            {
                Sender::SendMsg(connectSocket, input.second);
            }
        }

        void SendMoveRequest()
        {
            if (inputCol == -1 || inputRow == -1)
            {
                std::cout << Strings::ErrorClientMoveRequest << '\n';
                return;
            }
            Sender::SendRequestMove(connectSocket, inputCol, inputRow);
            state = ClientState::WaitMoveResponse;
        }

        void ReceiveMessages()
        {
            char buffer[INPUT_BUFFER_SIZE];
            while (running)
            {
                const int bytesReceived = recv(connectSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0)
                {
                    std::cout << "サーバーからの切断、またはエラーが発生しました。\n";
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
                default:
                    break;
                }
            }
        }

        void UpdateStone(Message message)
        {
            std::vector<int> response = MessageHandler::GetPlaceStone(message);
            if (response.size() != 3)
            {
                std::cout << "error: 002" << '\n';
                return;
            }
            reversiBoard.SetStone(response[1], response[2], static_cast<stone>(response[0]));
            reversiBoard.PrintBoard();
        }

        void GetMoveResponse(Message message)
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

        void RequireMoveInput()
        {
            std::cout << Strings::YourTurn << '\n';
            state = ClientState::WaitMoveRequest;
            inputCol = -1;
            inputRow = -1;
            std::cout << "縦(1~8):";
        }

        void GetPlayUserResponse(Message message)
        {
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
            std::cout << requestId << "からの対戦リクエストが承諾されました。\nゲームを始めます。\n";
            state = ClientState::InReversi;
            reversiBoard.Initialized();
            reversiBoard.PrintBoard();
        }

        void GetRequestUserPlay(Message message)
        {
            int requestedId = -1;
            MessageHandler::GetSingleIntValue(message, requestedId);
            std::cout << requestedId << "からの対戦リクエストが来ました。\n";
            std::cout << "対戦する場合は1,しない場合は0を入力してください\n入力:";
            state = ClientState::RequestedPlay;
            otherId = requestedId;
        }

        void GetUserList(Message message)
        {
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

        SOCKET SetupConnection(const std::string& ipAddress) const
        {
            const SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (connectSocket < 0)
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
                std::cout << "connectのエラー\n";
                closesocket(connectSocket);
                WSACleanup();
                return -1;
            }
            std::cout << "connect成功\n";
            return connectSocket;
        }
    };
} // winsoc

#endif //REVERSICLIENT_H
