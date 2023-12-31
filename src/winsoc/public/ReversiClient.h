//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSICLIENT_H
#define REVERSICLIENT_H
#include "../public/NetworkEntity.h"
#include <thread>

#include "MessageHandler.h"
#include "Sender.h"

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
            std::string userInput;
            while (running)
            {

                std::cin >> userInput;

                if (userInput == "end")
                {
                    running = false;
                    break;
                }
                Sender::SendMsg(connectSocket, userInput);
            }
            std::cout << "終了\n";
            receiveThread.join();
            closesocket(connectSocket);
            CleanupWinsock();
        }

    private:
        SOCKET connectSocket = 0;
        std::atomic<bool> running{true};

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
                switch (message.type)
                {
                case MessageType::RequestMessage:
                    message.CoutMessage();
                    break;
                case MessageType::Connected:
                    std::cout << "接続成功\n";
                    int id;
                    MessageHandler::GetConnectedId(message, id);
                    break;
                default:
                    break;
                }
            }
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
