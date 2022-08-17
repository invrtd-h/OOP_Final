//
// Created by 송혜근 on 2022/08/18.
//

#ifndef OOPFINAL_MINT_H
#define OOPFINAL_MINT_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

class Node;
class Trie;
class Clipboard;
class Document;
class Holder;
class TestHolder;
class StringHolder;
class Grimpan;
class TableHolder;
class LineHolder;
class Listener;

namespace mint_utils {

    template<typename T>
    auto identity_func = [](const T& t) -> T {
        return t;
    };

    std::string                 make_lowercase(std::string str);
    std::vector<std::string>    split(const std::string& str);
    inline std::string          reversed(std::string str);

    // TODO : move this function to Trie class
    unsigned int closeness(const std::string& s1, const std::string& s2,
                           const auto& init = identity_func<std::string>) {
        std::string str1 = init(s1);
        std::string str2 = init(s2);

        auto size = static_cast<unsigned int>(s1.size() < s2.size() ? s1.size() : s2.size());

        for (int ret = 0, i = 0; i < size; ++i) {
            if (str1[i] == str2[i]) {
                ++ret;
            } else {
                return ret;
            }
        }
    }

};

#endif //OOPFINAL_MINT_H
