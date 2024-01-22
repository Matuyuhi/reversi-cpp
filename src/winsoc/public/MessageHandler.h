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
    /**
     * @brief MessageのPayloadを解析するクラス
     * Sender classとセット
     */
    class MessageHandler
    {
    public:
        static int GetSingleIntValue(const Message& message, int& id);

        static int GetUserList(const Message& message, std::vector<int>& array);

        static std::pair<int, int> GetMoveInfo(const Message& message);

        static std::vector<int> GetPlaceStone(const Message& message);

    private:
        static PayloadType GetPayloadType(const Message& message);

        static std::any DeserializePayload(const Message& message);

        static std::vector<int> DeserializeIntArray(const std::string& payload);
    };
} // winsoc

#endif //WORK_MESSAGEHANDLER_H
