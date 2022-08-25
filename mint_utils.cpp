#include "mint_utils.h"

/**
 * 임의의 문자열을 입력으로 받아, 문자열의 모든 대문자를 소문자로 바꾸는 함수
 * @param str 임의의 문자열
 * @return 모든 대문자가 소문자로 바뀐 문자열
 */
std::string mints::make_lowercase(std::string str) {
    for (char &i: str) {
        if (65 <= i && i <= 90) {
            i += 32;
        }
    };
    return str;
}

std::vector<std::string> mints::split(const std::string &str) {
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

std::string mints::reversed(std::string str) {
    std::reverse(str.begin(), str.end());
    return str;
}

std::string mints::identity(std::string str) {
    return str;
}

const char *mints::named_exception::what() const noexcept {
    return err_msg.c_str();
}

mints::named_exception::named_exception(std::string &&s) : err_msg(s) {}

mints::named_exception::named_exception(const std::string &s) : err_msg(s) {}
