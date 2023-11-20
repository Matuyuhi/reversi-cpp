//
// Created by matuyuhi on 2023/11/08.
//

#ifndef WORK01_LIST_H
#define WORK01_LIST_H

#include <algorithm>
#include <iostream>
#include <list>
#include <functional>
#include <unordered_set>

struct PairHash {
    /// パフォーマンス向上のため、hashの衝突を減らす
    size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};
template<typename T>
class List {
    std::list<T> placeableCells;

    /// 要素の追跡用
    std::unordered_set<T, PairHash> cellSet = std::unordered_set<T, PairHash>(1000);

public:
    List() = default;
    // pointをリストに追加
    void add(const T& p) {
        /// セットに要素がない場合のみ追加
        if (cellSet.insert(p).second) {
            placeableCells.push_back(p);
        }
    }

    // 指定したpointをリストから削除
    bool remove(const T& p) {
        if (cellSet.erase(p)) {
            // セットから要素を削除
            // std::list::removeは直接要素を削除
            placeableCells.remove(p);
            return true;
        }
        return false;
    }

    // リストからすべての要素を削除
    void clear() {
        placeableCells.clear();
        cellSet.clear();
    }

    // pointがリストに含まれているか確認
    bool contains(const T& p) const {
        return cellSet.find(p) != cellSet.end();
    }

    // デバッグ用にリストの内容を出力
    void printList() const {
        for (const auto& p: placeableCells) {
            std::cout << "(" << p.first << ", " << p.second << ") ";
        }
        std::cout << std::endl;
    }

    int size() const {
        return placeableCells.size();
    }

    // リストの結合（他のリストの要素を追加）
    List& operator+=(const List& other) {
        for (const auto& item : other.placeableCells) {
            add(item);
        }
        return *this;
    }

    // 単一の要素の追加
    List& operator+=(const T& item) {
        add(item);
        return *this;
    }

    // イテレータ
    auto begin() -> decltype(placeableCells.begin()) {
        return placeableCells.begin();
    }

    auto end() -> decltype(placeableCells.end()) {
        return placeableCells.end();
    }

    // constイテレータ
    auto begin() const -> decltype(placeableCells.begin()) {
        return placeableCells.begin();
    }

    auto end() const -> decltype(placeableCells.end()) {
        return placeableCells.end();
    }
};


#endif //WORK01_LIST_H
