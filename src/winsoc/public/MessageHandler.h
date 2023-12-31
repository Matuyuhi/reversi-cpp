//
// Created by yuhi on 12/31/2023.
//

#ifndef WORK_MESSAGEHANDLER_H
#define WORK_MESSAGEHANDLER_H
#include <any>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Message.h"

namespace winsoc
{
    class MessageHandler
    {
    public:
        static int GetConnectedId(const Message& message, int& id)
        {
            try
            {
                std::any payload;
                DeserializePayload(message, payload);
                id = std::any_cast<int>(payload);
                return 0;
            }
            catch (const std::bad_any_cast& e)
            {
                std::cerr << e.what() << '\n';
                return -1;
            }
        }

    private:
        static PayloadType GetPayloadType(const Message& message)
        {
            switch (message.type)
            {
            case MessageType::GameStart:
                return PayloadType::Int;

            case MessageType::Connected:
            case MessageType::GameEnd:
            case MessageType::UserList:
            case MessageType::Move:
                return PayloadType::IntArray;

            case MessageType::RequestMove:
                return PayloadType::Coordinates;

            case MessageType::RequestGameEnd:
            case MessageType::Disconnected:
            case MessageType::WaitMove:
            case MessageType::Error:
            case MessageType::RequestMessage:
                return PayloadType::String;
            }
            return PayloadType::String;
        }

        static void DeserializePayload(const Message& message, std::any& payload)
        {
            switch (GetPayloadType(message))
            {
            case PayloadType::Int:
                payload = std::stoi(message.payload);
                break;
            case PayloadType::IntArray:
                payload = DeserializeIntArray(message.payload);
                break;
            case PayloadType::Coordinates:
                payload = DeserializeCoordinates(message.payload);
                break;
            case PayloadType::String:
                payload = message.payload;
                break;
            }
        }

        static std::pair<int, int> DeserializeCoordinates(const std::string& payload)
        {
            std::stringstream ss(payload);
            int x, y;
            char delimiter;
            ss >> x >> delimiter >> y;
            return std::make_pair(x, y);
        }

        static std::vector<int> DeserializeIntArray(const std::string& payload)
        {
            std::vector<int> array;
            std::stringstream ss(payload);
            std::string element;
            while (std::getline(ss, element, ','))
            {
                array.push_back(std::stoi(element));
            }
            return array;
        }
    };
} // winsoc

#endif //WORK_MESSAGEHANDLER_H
