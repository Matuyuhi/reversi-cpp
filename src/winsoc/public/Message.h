//
// Created by yuhi on 12/31/2023.
//

#ifndef WORK_MESSAGE_H
#define WORK_MESSAGE_H

namespace winsoc
{
    enum class MessageType
    {
        // server
        Connected,
        UserList,

        GameStart,
        Move,
        WaitMove,
        GameEnd,
        Error,

        RequestMove,
        RequestGameEnd,
        Disconnected,

        RequestMessage,
    };

    enum class PayloadType
    {
        Int,
        IntArray,
        Coordinates,
        String,
    };

    struct Message
    {
        MessageType type;
        std::string payload;

        static std::string GetTypeString(const Message& message)
        {
            switch (message.type)
            {
            case MessageType::GameStart:
                return "GameStart";

            case MessageType::Connected:
                return "Connected";

            case MessageType::GameEnd:
                return "GameEnd";

            case MessageType::UserList:
                return "UserList";

            case MessageType::Move:
                return "Move";

            case MessageType::RequestMove:
                return "RequestMove";

            case MessageType::RequestGameEnd:
                return "RequestGameEnd";

            case MessageType::Disconnected:
                return "Disconnected";

            case MessageType::WaitMove:
                return "WaitMove";

            case MessageType::Error:
                return "Error";
            case MessageType::RequestMessage:
                return "Message";
            }
            return "Unknown";
        }

        void CoutMessage() const
        {
            std::cout << "受信: " << GetTypeString(*this) << "**" << this->payload << '\n';
        }
    };
} // winsoc

#endif //WORK_MESSAGE_H
