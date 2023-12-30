//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSICLIENT_H
#define REVERSICLIENT_H
#include "../NetworkEntity.h"
#include <thread>

namespace server {

	class ReversiClient: public NetworkEntity {
	public:
		void Start() override {
			InitializeWinsock();
			connectSocket = SetupConnection("127.0.0.1"); // とりあえずlocalhostで。prodではしっかり設定すること

			char buffer[1024];
			const int bytesReceived = recv(connectSocket, buffer, 1024, 0); // 初回にIDを受信
			std::cout << "Server: " << std::string(buffer, bytesReceived) << '\n';

			std::thread receiveThread(&ReversiClient::ReceiveMessages, this);
			std::string userInput;
			while (running)
			{
				std::getline(std::cin, userInput);

				if (userInput == "end")
				{
					running = false;
					break;
				}

				send(connectSocket, userInput.c_str(), static_cast<int>(userInput.length()), 0);
			}
			std::cout << "終了\n";
			receiveThread.join();
			closesocket(connectSocket);
			CleanupWinsock();
		}

	private:
		SOCKET connectSocket = 0;
		std::atomic<bool> running{true};
		
		SOCKET SetupConnection(const std::string& ipAddress) {
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

		void ReceiveMessages()
		{
			char buffer[1024];
			while (running)
			{
				const int bytesReceived = recv(connectSocket, buffer, sizeof(buffer), 0);
				if (bytesReceived <= 0)
				{
					std::cout << "サーバーからの切断、またはエラーが発生しました。\n";
					running = false;
					break;
				}
				buffer[bytesReceived] = '\0'; // null終端を追加
				std::cout << "Server: " << buffer << '\n';
			}
		}

	};

} // server

#endif //REVERSICLIENT_H
