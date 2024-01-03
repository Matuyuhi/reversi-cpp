//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK01_STONE_H
#define WORK01_STONE_H

#include <string>
#include <iostream>

// リバーシの石の状態を表すenum
enum class stone
{
    Empty,
    Black,
    White
};

inline stone operator!(const stone s)
{
    switch (s)
    {
    case stone::Black: return stone::White;
    case stone::White: return stone::Black;
    default: return stone::Empty;
    }
}

inline std::string printStone(const stone s)
{
    switch (s)
    {
    case stone::Black: return "B ";
    case stone::White: return "W ";
    default: return " ";
    }
}

inline static std::string NamedStone(const stone s)
{
    switch (s)
    {
    case stone::Black:
        return "Black";
    case stone::White:
        return "White";
    default:
        return "Empty";
    }
}

inline std::ostream& operator<<(std::ostream& os, const stone s)
{
    return os << NamedStone(s);
}

#endif //WORK01_STONE_H
