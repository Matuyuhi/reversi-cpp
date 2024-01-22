//
// Created by yuhi on 12/31/2023.
//

#include "../public/MessageHandler.h"

namespace winsoc
{
    int MessageHandler::GetSingleIntValue(const Message& message, int& id)
    {
        try
        {
            const std::any payload = DeserializePayload(message);
            if (payload.type() == typeid(int))
            {
                id = std::any_cast<int>(payload);
                return 0;
            }

            throw std::bad_any_cast();
        }
        catch (const std::bad_any_cast& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
    }

    int MessageHandler::GetUserList(const Message& message, std::vector<int>& array)
    {
        try
        {
            const std::any payload = DeserializePayload(message);
            if (payload.type() == typeid(std::vector<int>))
            {
                array = std::any_cast<std::vector<int>>(payload);
                return 0;
            }

            throw std::bad_any_cast();
        }
        catch (const std::bad_any_cast& e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
    }

    std::pair<int, int> MessageHandler::GetMoveInfo(const Message& message)
    {
        try
        {
            const std::any payload = DeserializePayload(message);
            if (payload.type() == typeid(std::vector<int>))
            {
                auto list = std::any_cast<std::vector<int>>(payload);
                if (list.size() == 2)
                {
                    return {list[0], list[1]};
                }
                throw std::bad_any_cast();
            }

            throw std::bad_any_cast();
        }
        catch (const std::bad_any_cast& e)
        {
            std::cerr << e.what() << '\n';
            return {-1, -1};
        }
    }

    std::vector<int> MessageHandler::GetPlaceStone(const Message& message)
    {
        try
        {
            const std::any payload = DeserializePayload(message);
            if (payload.type() == typeid(std::vector<int>))
            {
                auto list = std::any_cast<std::vector<int>>(payload);
                if (list.size() == 3)
                {
                    return {list[0], list[1], list[2]};
                }
                throw std::bad_any_cast();
            }

            throw std::bad_any_cast();
        }
        catch (const std::bad_any_cast& e)
        {
            std::cerr << e.what() << '\n';
            return {-1, -1, -1};
        }
    }

    PayloadType MessageHandler::GetPayloadType(const Message& message)
    {
        switch (message.type)
        {
        case MessageType::Connected:
        case MessageType::GameStart:
        case MessageType::RequestConnectToPlayClient:
        case MessageType::FailConnectedPlayClient:
        case MessageType::UserPlayRequested:
        case MessageType::RequestGameStart:
            return PayloadType::Int;

        case MessageType::GameEnd:
        case MessageType::UserList:
        case MessageType::RequestMove:
        case MessageType::PlaceStone:
            return PayloadType::IntArray;

        case MessageType::RequestGameEnd:
        case MessageType::Disconnected:
        case MessageType::WaitMove:
        case MessageType::Error:
        case MessageType::RequestMessage:
        case MessageType::RequestUserList:
        case MessageType::ResponseMove:
            return PayloadType::String;
        }
        std::cout << "Unknown PayloadType\n";
        return PayloadType::String;
    }

    std::any MessageHandler::DeserializePayload(const Message& message)
    {
        switch (GetPayloadType(message))
        {
        case PayloadType::Int:
            return std::stoi(message.payload);
        case PayloadType::IntArray:
            return DeserializeIntArray(message.payload);
        case PayloadType::String:
            return message.payload;
        }
        return "";
    }

    std::vector<int> MessageHandler::DeserializeIntArray(const std::string& payload)
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
} // winsoc
