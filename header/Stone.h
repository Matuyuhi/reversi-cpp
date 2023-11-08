//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK01_STONE_H
#define WORK01_STONE_H

#include <string>
#include <iostream>
// リバーシの石の状態を表すenum
enum class stone {
    Empty,
    Black,
    White
};

inline stone operator!(stone s) {
    switch (s) {
        case stone::Black: return stone::White;
        case stone::White: return stone::Black;
        default: return stone::Empty;
    }
}

inline std::string printStone(stone s) {
    switch (s) {
        case stone::Black: return "B ";
        case stone::White: return "W ";
        default: return " ";
    }
}

#endif //WORK01_STONE_H
