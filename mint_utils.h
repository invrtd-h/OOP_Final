#ifndef OOPFINAL_MINT_UTILS_H
#define OOPFINAL_MINT_UTILS_H

#include <string>
#include <vector>
#include <algorithm>

namespace mints {

    std::string                         make_lowercase(std::string str);
    std::vector<std::string>            split(const std::string& str);
    std::string                         reversed(std::string str);
    std::string                         identity(std::string str);

    struct named_exception : std::exception {
        std::string                     err_msg;
        explicit                        named_exception(const std::string& s);
        explicit                        named_exception(std::string&& s);
        [[nodiscard]] const char*       what() const noexcept override;
    };

    struct double_alloc : named_exception {
        explicit double_alloc(std::string s) : named_exception(std::move(s)) {}
    };
    struct double_free : named_exception {
        explicit double_free(std::string s) : named_exception(std::move(s)) {}
    };
    struct input_out_of_range : named_exception {
        explicit input_out_of_range(std::string s) : named_exception(std::move(s)) {}
    };

    struct too_large_argument : named_exception {
        explicit too_large_argument(std::string s) : named_exception(std::move(s)) {}
    };

    // TODO : move this function to Trie class
    unsigned int closeness(const std::string& s1, const std::string& s2,
                           const auto& init = identity) {
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

        return size;
    }
}

#endif //OOPFINAL_MINT_UTILS_H
