//
// Created by yuhi on 12/31/2023.
//

#ifndef REVERSI_CPP_SENDER_H
#define REVERSI_CPP_SENDER_H
#include <any>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include "Message.h"

#pragma comment(lib, "ws2_32.lib")

namespace winsoc
{
    class Sender
    {
    public:
        static void SendConnected(const SOCKET& socket, int id)
        {
            Send(socket, Message{MessageType::Connected, std::to_string(id)});
        }

        static void SendMsg(const SOCKET& socket, const std::string& message)
        {
            Send(socket, Message{MessageType::RequestMessage, message});
        }

        static void SendMove(const SOCKET& socket, int user, int x, int y)
        {
            std::string payload = SerializeIntArray({user, x, y});
            Send(socket, Message{MessageType::Move, payload});
        }

        static void SendUserList(const SOCKET& socket, const std::vector<int>& array)
        {
            std::string payload = SerializeIntArray(array);
            Send(socket, Message{MessageType::UserList, payload});
        }

        /// <summary>
        /// 参加するclientに開始を宣言する
        /// </summary>
        static void SendGameStart(const SOCKET& socket, int id)
        {
            Send(socket, Message{MessageType::GameStart, std::to_string(id)});
        }

        static void SendWaitMove(const SOCKET& socket)
        {
            Send(socket, Message{MessageType::WaitMove, ""});
        }

        static void SendGameEnd(const SOCKET& socket, const std::vector<int>& array)
        {
            std::string payload = SerializeIntArray(array);
            Send(socket, Message{MessageType::GameEnd, payload});
        }

        static void SendError(const SOCKET& socket, const std::string& message)
        {
            Send(socket, Message{MessageType::Error, message});
        }

        static void SendRequestMove(const SOCKET& socket, int x, int y)
        {
            std::string payload = SerializeCoordinates(x, y);
            Send(socket, Message{MessageType::RequestMove, payload});
        }
        static void SendRequestUserList(const SOCKET& socket)
        {
            Send(socket, Message{MessageType::RequestUserList, ""});
        }
        
        /// <summary>
        /// サーバーを介してクライアントに接続要求を送信します。
        /// </summary>
        static void SendRequestPlayClient(const SOCKET& socket, int id)
        {
            Send(socket, Message{MessageType::RequestConnectToPlayClient, std::to_string(id)});
        }

        /// <summary>
        /// ユーザーに対してプレイ要求を送信します。
        /// </summary>
        static void SendUserPlayRequested(const SOCKET& socket, int id)
        {
            Send(socket, Message{MessageType::UserPlayRequested, std::to_string(id)});
        }

        /// <summary>
        /// ユーザーからのプレイ要求を拒否する
        /// </summary>
        static void FailConnectedPlayClient(const SOCKET& socket, int id)
        {
            Send(socket, Message{MessageType::FailConnectedPlayClient, std::to_string(id)});
        }

        /// <summary>
        /// プレイ要求を承諾してサーバーにゲームスタートのリクエストを送信する
        /// </summary>
        static void RequestGameStart(const SOCKET& socket, int otherId)
        {
            Send(socket, Message{MessageType::RequestGameStart, std::to_string(otherId)});
        }

        static Message Deserialize(const std::string& serialized)
        {
            std::stringstream ss(serialized);
            std::string typeStr;
            std::getline(ss, typeStr, '|');
            MessageType type = MessageTypeUtil::FromString(typeStr);
            std::string payload;
            std::getline(ss, payload);
            return Message{type, payload};
        }

    private:
        static void Send(const SOCKET& socket, const Message& message)
        {
            std::string serialized = Serialize(message);
            send(socket, serialized.c_str(), static_cast<int>(serialized.length()), 0);
        }

        static std::string Serialize(const Message& message)
        {
            std::stringstream ss;
            ss << MessageTypeUtil::ToString(message.type) << "|" << message.payload;
            return ss.str();
        }


        static std::string SerializeCoordinates(int x, int y)
        {
            std::stringstream ss;
            ss << x << "," << y;
            return ss.str();
        }

        static std::string SerializeIntArray(const std::vector<int>& array)
        {
            std::stringstream ss;
            for (size_t i = 0; i < array.size(); ++i)
            {
                ss << array[i];
                if (i < array.size() - 1) ss << ",";
            }
            return ss.str();
        }
    };
} // winsoc

#endif //REVERSI_CPP_SENDER_H
