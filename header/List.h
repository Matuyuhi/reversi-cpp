//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK01_LIST_H
#define WORK01_LIST_H

#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>

template <typename T>
class List {
private:
    std::vector<T> placeableCells;
public:
    /// pointをリストに追加
    void add(const T& p) {
        placeableCells.push_back(p);
    }

    /// 指定したpointをリストから削除
    bool remove(const T& p) {
        auto it = std::find(placeableCells.begin(), placeableCells.end(), p);
        if (it != placeableCells.end()) {
            placeableCells.erase(it);
            return true; // 削除成功
        }
        return false; // 要素が見つからない場合
    }

    /// リストからすべての要素を削除
    void clear() {
        placeableCells.clear();
    }

    /// pointがリストに含まれているか確認
    bool contains(const T& p) const {
        return std::find(placeableCells.begin(), placeableCells.end(), p) != placeableCells.end();
    }

    /// デバッグ用にリストの内容を出力
    void printList() const {
        for (const auto& p : placeableCells) {
            std::cout << "(" << p.first << ", " << p.second << ") ";
        }
        std::cout << std::endl;
    }

    int size() {
        return placeableCells.size();
    }

    /// インデックス演算子のオーバーロード
    T& operator[](int index) {
        return placeableCells[index];
    }
};


#endif //WORK01_LIST_H
