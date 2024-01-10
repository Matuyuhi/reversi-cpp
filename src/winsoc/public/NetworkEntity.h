//
// Created by matuyuhi on 2023/12/20.
//

#ifndef NETWORKENTITY_H
#define NETWORKENTITY_H
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WINSOCK_DEPRECATED_NO_WARNINGS

#define INPUT_BUFFER_SIZE 1024

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "../../reversi/public/Input.h"

#pragma comment(lib, "ws2_32.lib")

/// サーバー、クライアント共通の基底クラス
namespace winsoc
{
    class NetworkEntity
    {
    public:
        virtual void Start() = 0;

        virtual ~NetworkEntity() = default;

    protected:
        /// ポート番号
        int portNum = 9122;
        /// Winsockの初期化
        WSADATA wsaData;

        /// Winsockの初期化とportの指定
        void InitializeWinsock();

        /// Winsockの終了処理
        /// 必ず実行するようにする
        static void CleanupWinsock();
    };
}


#endif //NETWORKENTITY_H
