//
// Created by yuhi on 12/31/2023.
//

#ifndef WORK_MESSAGE_H
#define WORK_MESSAGE_H
#include <map>

namespace winsoc
{
    enum class Result
    {
        Success = 0,
        Error = 1,
        None = 2,
        
        // api有の結果
        GameEndWin = 3,
        GameEndLose = 4,
        GameEndDraw = 5,
    };

    enum class MessageType
    {
        // server
        Connected,
        UserList,
        UserPlayRequested,

        GameStart,
        ResponseMove,
        PlaceStone,
        WaitMove,
        GameEnd,
        Error,

        RequestUserList,
        RequestConnectToPlayClient,
        RequestGameStart,
        FailConnectedPlayClient,
        RequestMove,
        RequestGameEnd,
        Disconnected,

        RequestMessage,
        RequestInReversiMsg
    };

    enum class PayloadType
    {
        Int,
        IntArray,
        String,
    };

    class MessageTypeUtil
    {
    private:
        inline static const std::map<MessageType, std::string> messageTypeToString = {
            {MessageType::Connected, "Connected"},
            {MessageType::UserList, "UserList"},
            {MessageType::UserPlayRequested, "UserPlayRequested"},

            {MessageType::GameStart, "GameStart"},
            {MessageType::ResponseMove, "Move"},
            {MessageType::PlaceStone, "PlaceStone"},
            {MessageType::WaitMove, "WaitMove"},
            {MessageType::GameEnd, "GameEnd"},
            {MessageType::Error, "Error"},

            {MessageType::RequestUserList, "RequestUserList"},
            {MessageType::RequestConnectToPlayClient, "RequestConnectToPlayClient"},
            {MessageType::RequestGameStart, "RequestGameStart"},
            {MessageType::FailConnectedPlayClient, "FailConnectedPlayClient"},
            {MessageType::RequestMove, "RequestMove"},
            {MessageType::RequestGameEnd, "RequestGameEnd"},
            {MessageType::Disconnected, "Disconnected"},

            {MessageType::RequestMessage, "RequestMessage"},
            {MessageType::RequestInReversiMsg, "RequestInReversiMsg"}
        };

    public:
        static std::string ToString(MessageType type)
        {
            auto it = messageTypeToString.find(type);
            if (it != messageTypeToString.end())
            {
                return it->second;
            }
            return "Unknown";
        }

        static MessageType FromString(const std::string& str)
        {
            for (const auto& pair : messageTypeToString)
            {
                if (pair.second == str)
                {
                    return pair.first;
                }
            }
            return MessageType::Error;
        }
    };

    struct Message
    {
        MessageType type;
        std::string payload;
        Result result = Result::None;

        Message(MessageType type, std::string payload)
        {
            this->type = type;
            this->payload = payload;
        }

        Message(MessageType type, std::string payload, Result result)
        {
            this->type = type;
            this->payload = payload;
            this->result = result;
        }

        Message(MessageType type, std::string payload, int resultCode)
        {
            this->type = type;
            this->payload = payload;
            this->result = static_cast<Result>(resultCode);
        }

        void CoutMessage() const
        {
            std::cout << "受信: " << MessageTypeUtil::ToString(this->type) << "**" << this->payload << '\n';
        }
    };
} // winsoc

#endif //WORK_MESSAGE_H
