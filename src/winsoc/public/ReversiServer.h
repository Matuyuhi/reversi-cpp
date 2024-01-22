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
#include "../../reversi/public/ReversiAI.h"
#include "SessionInfo.h"
#include "ClientInfo.h"


namespace winsoc
{
    class ReversiServer : public NetworkEntity
    {
    private:
        std::unordered_map<int, ClientInfo*> clientSockets;
        std::mutex socketsMutex;
        std::unordered_map<int, SessionInfo*> sessions;

        const int maxConnections = 6;

        /**
         * クライアントを追加する
         * @param client 作成したクライアント
         */
        void AddClient(ClientInfo* client);

        /**
         * セッションを追加する
         * @param session 作成したセッション
         */
        void AddSession(SessionInfo* session);

        /**
         * クライアントを削除する
         * @param clientId
         */
        void DeleteClient(int clientId);

        /**
         * クライアントを取得する
         * @param clientId
         * @return クライアント
         */
        ClientInfo* GetClient(int clientId);

        /**
         * クライアントの受信を処理する
         * @param clientId
         */
        void HandleClient(int clientId);

        /**
         * リバーシのマスを置く
         * @param clientId
         * @param message
         */
        void OnMove(int clientId, const Message& message) const;

        /**
         * ユーザーリストを送る
         * @param clientId
         */
        void SendUserList(int clientId) const;

        /**
         * 全てのクライアントにメッセージを送る
         * @param ownerId
         * @param message
         */
        void SendAllClient(int ownerId, const std::string& message) const;

        /**
         * リバーシの相手にメッセージを送る
         * @param clientId
         * @param message
         */
        void RequestReversiMessage(int clientId, Message& message) const;


        /**
         * ユーザーの対戦リクエスト
         * @param clientId
         * @param message
         */
        void UserPlayConnectRequest(int clientId, Message& message) const;

        /**
         * ユーザーの対戦リクエストに対するレスポンス
         * @param clientId
         * @param message
         */
        void UserPlayConnectResponse(int clientId, Message& message);

        void UserPlayVsCOM(int clientId);

        /**
         * \brief クライアントからの強制終了
         * \param clientId 
         * \param message 
         */
        void RequestQuitGame(int clientId, const Message& message) const;


        /**
         * ゲームを開始する
         * @param a clientId
         * @param b clientId
         */
        void GameStart(int a, int b);

        /**
         * setup listening socket
         * @return
         */
        SOCKET SetupListeningSocket() const;

        /**
         * close client connection
         * @param clientId
         */
        void CloseClientConnection(int clientId);

    public:
        ~ReversiServer() override;

        void Start() override;
    };
} // winsoc

#endif //REVERSISERVER_H
