//
// Created by yuhi on 2024/01/22.
//

#ifndef REVERSI_CPP_SESSIONINFO_H
#define REVERSI_CPP_SESSIONINFO_H
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
#include "ClientInfo.h"

namespace winsoc {

    struct SessionInfo
    {
        ClientInfo* clients[2]{};
        int currentTurn = -1;
        int sessionId = -1;
        ReversiSessionManager board;

        SessionInfo();

        SessionInfo(ClientInfo* clientA, ClientInfo* clientB, int sessionId);

        void Initialized();

        void OnMove(int clientId, int row, int col);

        void ErrorDisconnect(int clientId);

        bool IsClient(int clientId);

        static stone ClientStone(int num);

        bool operator==(const SessionInfo& other) const;

    private:
        void SendToggleTurn();

        void SendFinished();
    };

} // winsoc

#endif //REVERSI_CPP_SESSIONINFO_H
