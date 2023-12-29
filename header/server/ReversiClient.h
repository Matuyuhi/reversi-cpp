//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSICLIENT_H
#define REVERSICLIENT_H
#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")
#include "NetworkEntity.h"

namespace server {

	class ReversiClient: public NetworkEntity {
	public:
		void Start() override {
			InitializeWinsock();
			SOCKET connectSocket = SetupConnection("127.0.0.1", portNum); // 例としてlocalhostの12345ポートを使用

			std::string message = "Hello, Server!";
			send(connectSocket, message.c_str(), message.length(), 0);

			char buffer[1024];
			int bytesReceived = recv(connectSocket, buffer, 1024, 0);
			std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;

			closesocket(connectSocket);
			CleanupWinsock();
		}

	private:
		SOCKET SetupConnection(const std::string& ipAddress, int port) {
			SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in serverAddr;
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(port);
			serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

			connect(connectSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

			return connectSocket;
		}

		void CleanupWinsock() {
			WSACleanup();
		}

	};

} // server

#endif //REVERSICLIENT_H
