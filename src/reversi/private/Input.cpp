//
// Created by matuyuhi on 2023/12/20.
//

#include "../public/Input.h"

std::pair<int, std::string> Input::getInputNumbers()
{
    std::string input;
    std::cin >> input;
    if (std::isdigit(input[0]))
    {
        // 文字列からint型に変換を試みる
        int number;
        std::stringstream ss(input);
        if (ss >> number)
        {
            // 変換成功したら、intと元の文字列のペアを返す
            return std::make_pair(number, "");
        }
    }
    // 変換できなければ、文字列のみを返す
    return std::make_pair(INPUT_ERROR_NUMBER, input);
}
