#ifndef OOPFINAL_MINT_UTILS_H
#define OOPFINAL_MINT_UTILS_H

#include <string>
#include <vector>
#include <algorithm>

namespace mint_utils {

    template<typename T>
    auto identity_func = [](const T& t) -> T {
        return t;
    };

    std::string                 make_lowercase(std::string str);
    std::vector<std::string>    split(const std::string& str);
    std::string                 reversed(std::string str);

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
}

#endif //OOPFINAL_MINT_UTILS_H
