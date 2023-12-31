//
// Created by matuyuhi on 2023/12/20.
//

#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#define INPUT_ERROR_NUMBER 12192438

class Input {
public:
    /// 入力を受け取る
    /// @return キャストした数値&入力文字列を返す
    static std::pair<int, std::string> getInputNumbers();

};



#endif //INPUT_H
