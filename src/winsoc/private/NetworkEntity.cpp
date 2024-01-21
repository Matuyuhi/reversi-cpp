//
// Created by matuyuhi on 2023/12/20.
//

#include "../public/NetworkEntity.h"

void winsoc::NetworkEntity::InitializeWinsock()
{
    /// Winsockの初期化
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }
}

void winsoc::NetworkEntity::CleanupWinsock()
{
    WSACleanup();
}
