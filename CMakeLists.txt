cmake_minimum_required(VERSION 3.22.1)
project(work)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(work src/main.cpp
        header/Stone.h
        src/ReversiBoard.cpp
        header/ReversiBoard.h
        header/List.h
        header/cout.h
        src/ReversiGame.cpp
        header/ReversiGame.h
        src/ReversiAI.cpp
        header/ReversiAI.h
        header/IPlayer.h
        src/AIPlayer.cpp
        header/AIPlayer.h
        src/InputPlayer.cpp
        header/InputPlayer.h
        header/TurnState.h
)
