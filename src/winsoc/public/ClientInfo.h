//
// Created by yuhi on 2024/01/22.
//

#ifndef REVERSI_CPP_CLIENTINFO_H
#define REVERSI_CPP_CLIENTINFO_H

#include "NetworkEntity.h"

namespace winsoc
{
    enum ClientState
    {
        Idle,
        InReversi,
        Disconnect,
        None
    };

    using ClientInfo = struct _ClientInfo
    {
        SOCKET socket;
        int id;
        ClientState state;
    };
    inline static ClientInfo ClientNone = ClientInfo{0, -1, Disconnect};
}


#endif //REVERSI_CPP_CLIENTINFO_H
