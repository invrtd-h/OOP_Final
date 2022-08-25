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
            vecstr.back() += static_cast<char>(c + 32);
        } else if (97 <= c && c <= 122) {
            vecstr.back() += c;
        } else if (!vecstr.back().empty()) {
            vecstr.emplace_back("");
        }
    }
    return vecstr;
}

/**
 * 입력 그대로 리턴하는 함수
 * @param str 임의의 문자열
 * @return 입력받은 그대로의 문자열
 */
std::string mints::identity_str(std::string str) {
    return str;
}

/**
 * 입력을 뒤집어 리턴하는 함수
 * @param str 임의의 문자열
 * @return 입력을 뒤집은 문자열
 */
std::string mints::reversed_str(std::string str) {
    std::ranges::reverse(str);
    return str;
}

const char *mints::named_exception::what() const noexcept {
    return err_msg.c_str();
}

/**
 * named_exception 생성자
 * @param s std::string&& : 예외 사유
 */
mints::named_exception::named_exception(std::string &&s) : err_msg(s) {}

mints::named_exception::named_exception(const std::string &s) : err_msg(s) {}
