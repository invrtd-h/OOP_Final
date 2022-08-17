#include "mint.h"

std::string mint_utils::make_lowercase(std::string str) {
    for (int i = 0; i < str.size(); ++i) {
        if (65 <= str[i] && str[i] <= 90) {
            str[i] += 32;
        }
    }
    return str;
}

std::vector<std::string> mint_utils::split(const std::string &str) {
    std::vector<std::string> vecstr(1);
    for (char c : str) {
        if (65 <= c && c <= 90) {
            vecstr.back() += c + 32;
        } else if (97 <= c && c <= 122) {
            vecstr.back() += c;
        } else if (!vecstr.back().empty()) {
            vecstr.emplace_back("");
        }
    }
    return vecstr;
}

std::string mint_utils::reversed(std::string str) {
    std::reverse(str.begin(), str.end());
    return str;
}
