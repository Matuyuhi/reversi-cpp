//
// Created by matuyuhi on 2023/11/08.
//

#include "../public/ReversiGame.h"
#include <format>

ReversiGame::~ReversiGame()
{
    delete players[stone::Black];
    delete players[stone::White];
}

ReversiGame::ReversiGame(): ReversiBoard(boardSize)
{
    /* プレイヤーの設定 */
    players[playerStone] = new InputPlayer(this, stone::Black);
    players[!playerStone] = new AIPlayer(this, stone::White);
    Clear();
}

void ReversiGame::Clear()
{
    Initialized();
    /* ゲーム開始時の4つの石を配置 */
    board[boardSize / 2 - 1][boardSize / 2 - 1] = stone::White;
    board[boardSize / 2 - 1][boardSize / 2] = stone::Black;
    board[boardSize / 2][boardSize / 2 - 1] = stone::Black;
    board[boardSize / 2][boardSize / 2] = stone::White;

    /* 配置した位置に対しておける位置を更新 */
    updatePlaceableCellsInList(stone::White);
    updatePlaceableCellsInList(stone::Black);
}


ReversiGame::FinishedState ReversiGame::Start()
{
    srand(time(nullptr));
    // 現在のプレイヤーを黒に設定する
    auto currentPlayer = stone::Black;
    printBoard(currentPlayer);

    while (!finished())
    {
        spacer();
        spacer();
        std::cout << "//* " << (currentPlayer == playerStone ? "あなた" : "相手") << "のターン *//" << '\n';
        TurnState state = players[currentPlayer]->make();
        if (state == TurnState::Quit)
        {
            std::cout << "ゲームを強制終了します" << '\n';
            break;
        }

        // 次のプレイヤーに交代する
        currentPlayer = !currentPlayer;
        printBoard(currentPlayer);
    }
    gameFinished();
    spacer();
    spacer();
    std::cout << "ゲームを終了しますか?" << '\n';
    std::cout << "q: 終了する r: もう一度" << '\n';
    while (true)
    {
        std::cout << "入力: ";
        std::string input;
        std::cin >> input;
        if (input == "q")
        {
            return FinishedState::Quit;
        }
        if (input == "r")
        {
            return FinishedState::Restart;
        }
        std::cout << "入力が不正です" << '\n';
    }
    return FinishedState::Error;
}

void ReversiGame::gameFinished()
{
    spacer();
    // 結果を出力する
    if (!getStoneCount(stone::Empty))
    {
        // 空きマスがない場合(正常な終了)
        if (getStoneCount(stone::Black) > getStoneCount(stone::White))
        {
            std::cout << "あなたの勝ちです" << '\n';
        }
        else if (getStoneCount(stone::Black) < getStoneCount(stone::White))
        {
            std::cout << "あなたの負けです" << '\n';
        }
        else
        {
            std::cout << "引き分けです" << '\n';
        }
    }
    else if (getPlaceableCells(stone::Black).size() == 0 || getPlaceableCells(stone::White).size() == 0)
    {
        // 空きマスがある場合(異常終了)
        std::cout << "おけるマスがないため、ゲームを終了します" << '\n';
        if (getStoneCount(stone::Black) > getStoneCount(stone::White))
        {
            std::cout << "あなたの勝ちです" << '\n';
        }
        else if (getStoneCount(stone::Black) < getStoneCount(stone::White))
        {
            std::cout << "あなたの負けです" << '\n';
        }
        else
        {
            std::cout << "引き分けです" << '\n';
        }
    }
    spacer();
    std::map<stone, int> stonesCount = getStonesCount();
    std::cout << std::format("{:<21} {}\n", "プレイヤーの石の数:", stonesCount[stone::Black]);
    std::cout << std::format("{:<21} {}\n", "COMの石の数:", stonesCount[stone::White]);
    spacer();
}
