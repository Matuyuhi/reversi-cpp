//
// Created by yuhi on 1/3/2024.
//

#ifndef WORK_STRINGS_H
#define WORK_STRINGS_H
#include <string>

#include "Message.h"
#include "../../reversi/public/Stone.h"

namespace winsoc
{
    class Strings
    {
    public:
        inline static std::string OtherPlayerTurn = "相手のターンです";
        inline static std::string YourTurn = "あなたのターンです";
        inline static std::string NotYourTurn = "あなたのターンではありません";

        inline static std::string WaitServer = "サーバーからの応答を待っています";

        inline static std::string ErrorClientMoveRequest = "入力を完了せずにリクエストを送ろうとしています";

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

        inline static std::string NotInSession = "セッションに参加していません";

        static std::string CallClientStone(const stone stone)
        {
            return "あなたの色は" + NamedStone(stone) + "です";
        }

        static std::string FailStartGame(int otherId)
        {
            return otherId + "からリクエストが拒否されました。";
        }

        static std::string FailUserConnect(int otherId)
        {
            return otherId + "は現在遊べる状態ではないです。";
        }

        static std::string SuccessStartGame(int otherId)
        {
            return otherId + "からリクエストが承諾されました。";
        }

    private:
        static std::string Named(bool mine)
        {
            return mine ? "あなたが" : "相手が";
        }
    };
} // winsoc

#endif //WORK_STRINGS_H
