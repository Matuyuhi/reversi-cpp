//
// Created by yuhi on 2024/01/22.
//

#include "../public/SessionInfo.h"

namespace winsoc {
    SessionInfo::SessionInfo() {
        clients[0] = &ClientNone;
        clients[1] = &ClientNone;
        sessionId = -1;
        board = ReversiSessionManager();
    }

    void SessionInfo::Initialized() {
        board = ReversiSessionManager();
        board.Init();
        currentTurn = 0;
        SendToggleTurn();
    }

    SessionInfo::SessionInfo(ClientInfo *clientA, ClientInfo *clientB, int sessionId) {
        clients[0] = clientA;
        clients[1] = clientB;
        this->sessionId = sessionId;
        board = ReversiSessionManager();
    }

    void SessionInfo::OnMove(int clientId, int row, int col) {
        const ClientInfo* client = clients[currentTurn];
        const ClientInfo* other = clients[currentTurn == 0 ? 1 : 0];
        const stone stone = ClientStone(currentTurn);
        if (client->id != clientId && other->id == clientId)
        {
            /// 例外
            Sender::SendMsg(client->socket, Strings::NotYourTurn);
            return;
        }
        if (!board.isCanPlace(row, col, stone))
        {
            Sender::ResponseMove(client->socket, Strings::PlaceStoneResult(col, row, true, Result::Error),
                                 Result::Error);
            return;
        }
        board.SetStone(row, col, stone);
        Sender::ResponseMove(client->socket, Strings::PlaceStoneResult(col, row, true, Result::Success),
                             Result::Success);
        Sender::SendPlaceStone(client->socket, static_cast<int>(stone), row, col);
        Sender::ResponseMove(other->socket, Strings::PlaceStoneResult(col, row, false, Result::Success),
                             Result::Success);
        Sender::SendPlaceStone(other->socket, static_cast<int>(stone), row, col);
        SendToggleTurn();
    }

    void SessionInfo::ErrorDisconnect(int clientId) {
        for (auto client : clients)
        {
            if (client->id != clientId)
            {
                Sender::SendMsg(client->socket, "相手が切断しました\nAIと戦います");
                SendToggleTurn();
            }
        }
    }

    bool SessionInfo::IsClient(int clientId) {
        return clients[0]->id == clientId || clients[1]->id == clientId;
    }

    stone SessionInfo::ClientStone(int num) {
        return num == 0 ? stone::Black : stone::White;
    }

    bool SessionInfo::operator==(const SessionInfo &other) const {
        return sessionId == other.sessionId;
    }

    void SessionInfo::SendToggleTurn() {
        if (board.finished())
        {
            SendFinished();
            return;
        }
        if (clients[currentTurn]->state == InReversi) {
            Sender::SendMsg(clients[currentTurn]->socket, Strings::OtherPlayerTurn);
        }

        // ターンの切り替え
        currentTurn = currentTurn == 0 ? 1 : 0;
        if (board.getPlaceableCells(ClientStone(currentTurn)).size() == 0) {
            Sender::SendMsg(clients[currentTurn]->socket, "おけるマスがないので相手にターンになります");
            Sender::SendMsg(clients[currentTurn == 0 ? 1 : 0]->socket, "相手のおけるマスがないのであなたのターンです");
            SendToggleTurn();
            return;
        }
        if (clients[currentTurn]->state == InReversi)
        {
            Sender::SendWaitMove(clients[currentTurn]->socket);
        } else {
            auto ai = ReversiAI(board, ClientStone(currentTurn));
            // ランダムに置ける場所を探す
            const std::pair<int, int> cell = ai.chooseMove();
            OnMove(clients[currentTurn]->id, cell.first, cell.second);
        }
    }

    void SessionInfo::SendFinished() {
        int index = 0;
        int counts[] = {
                board.getStoneCount(ClientStone(0)),
                board.getStoneCount(ClientStone(1))
        };
        for (auto client : clients)
        {
            if (!board.getStoneCount(stone::Empty))
            {
                // 空きマスがない場合
                Sender::SendMsg(client->socket, "ゲームを終了します");
            }
            else if (board.getPlaceableCells(stone::Black).size() == 0 || board.getPlaceableCells(stone::White).size() == 0)
            {
                // 空きマスがある場合
                Sender::SendMsg(client->socket, "おけるマスがないため、ゲームを終了します");
            }

            if (counts[index] > counts[!index])
            {
                Sender::SendGameEnd(client->socket, "あなたの勝ちです", Result::GameEndWin);
            }
            else if (counts[index] < counts[!index])
            {
                Sender::SendGameEnd(client->socket,"あなたの勝ちです", Result::GameEndLose);
            }
            else
            {
                Sender::SendGameEnd(client->socket, "あなたの勝ちです", Result::GameEndDraw);
            }
            index++;
        }

    }
} // winsoc