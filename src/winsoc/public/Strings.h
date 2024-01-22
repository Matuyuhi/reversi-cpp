//
// Created by yuhi on 1/3/2024.
//

#ifndef WORK_STRINGS_H
#define WORK_STRINGS_H
#include <format>
#include <string>

#include "Message.h"
#include "../../reversi/public/Stone.h"

namespace winsoc
{
    /**
     * \brief strings resource
     */
    class Strings
    {
    public:
        inline static int refreshUserListNum = 0;
        inline static int vsComNum = 9;
        inline static std::string AutoPlaceStg = "a";
        inline static std::string RequestQuitGame = "q";
        inline static std::string End = "end";

        inline static std::string SelectEntity = "サーバーかクライアントかを選んでください";

        inline static std::string OtherPlayerTurn = "相手のターンです";
        inline static std::string YourTurn = "あなたのターンです";
        inline static std::string AutoNumberIs = AutoPlaceStg + "でランダムなマスに置きます";
        inline static std::string QuitGameIs = RequestQuitGame + "でゲームを終了します";
        inline static std::string NotYourTurn = "あなたのターンではありません";

        inline static std::string WaitServer = "サーバーからの応答を待っています";

        inline static std::string SendDisconnect = "サーバーに切断を通知しています";

        inline static std::string GameStartOk = "対戦を承諾しました";
        inline static std::string GameStartNg = "対戦を拒否しました";

        inline static std::string InputRow = "横(1~8):";
        inline static std::string InputCol = "縦(1~8):";

        inline static std::string InputFailed = "入力に失敗しました";
        inline static std::string InputFormatFailed = "入力が不正です";
        inline static std::string Require0or1Error = "0か1で入力してください";

        inline static std::string ErrorClientMoveRequest = "入力を完了せずにリクエストを送ろうとしています";

        inline static std::string NotFoundUser = "ユーザーが見つかりません";

        inline static std::string NotInSession = "セッションに参加していません";

        inline static std::string NotConnection = "接続できませんでした";

        inline static std::string ServerConnectError = "サーバーからの切断、またはエラーが発生しました。\n続行するには何かキーを押してください";

        inline static std::string SuccessConnect = "接続成功";

        inline static std::string BadResponse = "bad response";

        inline static std::string StartFailedPleaseReselect = "始められませんでした。もう一度遊ぶ相手を選んでください";

        inline static std::string UserList = "ユーザーリスト";

        inline static std::string NavRefreshUserList =
            "更新するには" + std::to_string(refreshUserListNum) +" ,遊ぶ場合は相手のClientIdを入力してください\n" +
                std::to_string(vsComNum) + "でCOMと遊べます";

        inline static std::string SocketOpenError = "ソケットオープンエラー";

        inline static std::string AlreadyOpenedPortError = "ポートが既に使用されています";

        inline static std::string HostName = "Host name";

        inline static std::string IpAddr = "IP Address(es)";

        inline static std::string ListenPort = "Listening on port";

        inline static std::string GetHostNameError = "get hostname failed with error";

        inline static std::string ServerSuccessListen = "listen成功\nクライアント側で表示されたipアドレスとポート番号を入力してください";

        inline static std::string AcceptFailed = "Accept failed";

        inline static std::string MaxClientCountError = "クライアントの接続数が上限に達しています";

        inline static std::string AddedNewClient = "新しいClientが追加されました";

        inline static std::string StartVsCOM = "COMとはじめます";

        static std::string DisconnectedClient(int id)
        {
            return "クライアント(Id: " + std::to_string(id) + ")が切断しました。";
        }

        static std::string ClientInputIP(std::string defaultIP)
        {
            return "IPアドレスを入力してください（Enterでデフォルト：" + defaultIP + "）: ";
        }

        static std::string ClientInputPort(std::string defaultPort)
        {
            return "ポート番号を入力してください（Enterでデフォルト：" + defaultPort + "）: ";
        }

        static std::string ServerConnectedMessage(int userId)
        {
            return "あなたのIDは" + std::to_string(userId) + "です。\n" + End + "と入力するといつでも終了できます";
        }

        static std::string StoneCount(bool isMine, int count)
        {
            return std::format("{:<21} {}\n", isMine ? "プレイヤーの石の数:" : "相手の石の数:", count);
        }

        static std::string SendUserMessageFormat(int clientId, std::string message)
        {
            return "client" + std::to_string(clientId) + ":" + message;
        }

        static std::string Error(int code)
        {
            return "Error" + std::to_string(code + baseErrorCode);
        }

        static std::string PlaceStoneResult(int col, int row, bool mine, Result result)
        {
            col++;
            row++;
            if (result == Result::Success)
            {
                return Named(mine) + std::to_string(col) + "," + std::to_string(row) + "に置きました";
            }
            if (result == Result::Error)
            {
                // 自分にしか送らないので名指しは避ける
                return std::to_string(col) + "," + std::to_string(row) + "に置けませんでした";
            }
            return "予期せぬエラーが発生しました";
        }

        static std::string CallClientStone(const stone stone)
        {
            return "あなたの色は" + NamedStone(stone) + "です";
        }

        static std::string SendGameRequest(int findId)
        {
            return std::to_string(findId) + "に対して対戦リクエストを送ります";
        }

        static std::string FailStartGame(int otherId)
        {
            return std::to_string(otherId) + "からリクエストが拒否されました。";
        }

        static std::string FailUserConnect(int otherId)
        {
            return std::to_string(otherId) + "は現在遊べる状態ではないです。";
        }

        static std::string SuccessStartGame(int otherId)
        {
            return std::to_string(otherId) + "からリクエストが承諾されました。";
        }

        static std::string GameResultMessage(Result result)
        {
            if (result == Result::GameEndWin)
            {
                return "あなたの勝ちです";
            }
            if (result == Result::GameEndLose)
            {
                return "あなたの負けです";
            }
            if (result == Result::GameEndDraw)
            {
                return "引き分けです";
            }
            if (result == Result::None)
            {
                return "切断しました";
            }
            return "予期せぬエラーが発生しました";
        }

        static std::string GameRequested(int userId)
        {
            return std::to_string(userId) + "からの対戦リクエストが来ました。\n対戦する場合は1,しない場合は0を入力してください\n入力:";
        }

    private:
        inline static int baseErrorCode = 10000;

        static std::string Named(bool mine)
        {
            return mine ? "あなたが" : "相手が";
        }
    };
} // winsoc

#endif //WORK_STRINGS_H
