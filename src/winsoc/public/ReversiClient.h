//
// Created by matuyuhi on 2023/12/20.
//

#ifndef REVERSICLIENT_H
#define REVERSICLIENT_H
#include "../public/NetworkEntity.h"
#include <thread>
#include <format>

#include "MessageHandler.h"
#include "Sender.h"
#include "ReversiSessionManager.h"
#include "Strings.h"

namespace winsoc
{
    class ReversiClient : public NetworkEntity
    {
    public:
        ~ReversiClient() override = default;

        void Start() override;

    private:
        enum class ClientState
        {
            InReversi,
            WaitMoveRequest,
            WaitMoveResponse,
            Idle,
            RequestedPlay,
        };

        SOCKET connectSocket = -1;
        ClientState state = ClientState::Idle;
        std::atomic<bool> running{true};
        int userId = -1;
        int otherId = -1;
        std::vector<int> currentUserList;
        ReversiSessionManager reversiBoard = ReversiSessionManager();
        stone color = stone::Empty;

        // 0~7
        int inputCol = -1;
        // 0~7
        int inputRow = -1;

        /**
         * ゲーム中かどうか
         * @return
         */
        bool IsInReversi();

        /**
         * ユーザーからの入力を受け取る関数
         * @param prompt
         * @return
         */
        void InputHandler(const std::pair<int, std::string>& input);

        /**
         * 対戦リクエストを受け取ったときの入力ハンドラ
         * @param input
         */
        void RequestPlayInputHandler(const std::pair<int, std::string>& input);

        /**
         * 待機中の入力ハンドラ
         * 主にメッセージ送信のみ
         * @param input
         */
        void IdleInputHandler(const std::pair<int, std::string>& input) const;

        /**
         * ゲーム中の入力ハンドラ
         * @param input
         */
        void InReversiInputHandler(const std::pair<int, std::string>& input);

        /**
         * InReversiInputHandlerで呼び出される関数
         * 入力が終わったらrow, colをサーバーに送信する
         * @param input
         */
        void SendMoveRequest();

        /**
         * サーバーからのメッセージを受け取る関数
         */
        void ReceiveMessages();

        /**
         * サーバーから送られたリザルトをもとにゲームを終了する
         * @param message
         */
        void FinishedGame(const Message& message);

        /**
         * サーバーから送られたメッセージをもとに石を置く
         * @param message
         */
        void UpdateStone(const Message& message);

        /**
         * 正常に石を置けたかどうかでstateを変更する
         * 置けないマスを指定した場合は、もう一度入力させる
         * @param message
         */
        void GetMoveResponse(const Message& message);

        /**
         * マスの入力情報を初期化する
         * @param message
         */
        void RequireMoveInput();

        /**
         * プレイを開始するかを受け取り処理する
         * ここでclient側のboardの設定もする
         * @param message
         */
        void GetPlayUserResponse(const Message& message);

        /**
         * 他から対戦リクエストを受け取った時の処理
         * stateを変える
         * @param message
         */
        void GetRequestUserPlay(const Message& message);

        /**
         * ユーザーリストを受け取り、表示する
         * @param message
         */
        void GetUserList(const Message& message);

        /**
         * clientのセットアップを行う
         * @param prompt
         * @return
         */
        SOCKET SetupConnection() const;
    };
} // winsoc

#endif //REVERSICLIENT_H
