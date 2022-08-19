#include "mint_utils.h"

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

mints::double_alloc::double_alloc(std::string s) : std::exception(), err_msg(std::move(s)) {}

const char *mints::double_alloc::what() const noexcept {return err_msg.c_str();}

mints::double_free::double_free(std::string s) : std::exception(), err_msg(std::move(s)) {}

const char *mints::double_free::what() const noexcept {return err_msg.c_str();}
