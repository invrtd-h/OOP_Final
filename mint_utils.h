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
    struct unpoppable : named_exception {
        explicit unpoppable(std::string s) : named_exception(std::move(s)) {}
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

namespace mint_manuals {
    const std::string print_manual = "Put 1 to print all CONTENTS in the document.\n"
                                     "Put 2 to print all TYPES and TITLES of holders in the document.\n"
                                     "\n"
                                     "Put 10 to print an individual holder.\n"
                                     "Put 11 to access and modify an individual holder.\n"
                                     "\n"
                                     "Put 20 to make a new holder at the END of the document.\n"
                                     "Put 21 to add a new holder BETWEEN holders and holders.\n"
                                     "Put 22 to pop a holder at the END of the document.\n"
                                     "Put 23 to delete a holder in any place.\n"
                                     "\n"
                                     "Put -1 to exit the program.\n\n";

    const std::string str_manual = "Put 1 to print all CONTENTS of the holder.\n"
                                   "Put 3 to change the title of holder.\n"
                                   "\n"
                                   "Put 10 to push your text at the back of the string.\n"
                                   "Put 11 to insert your text between the data.\n"
                                   "Put 20 to delete some elements between strings.\n"
                                   "Put 21 to replace some elements between string into other.\n"
                                   "Put 50 to print title.\n"
                                   "Put 51 to hide title.\n"
                                   "\n"
                                   "Put 99 to operate the spell-check function.\n"
                                   "Put -1 to break.\n\n";

    const std::string tab_manual = "Put 1 to print all CONTENTS of the holder.\n"
                                   "Put 3 to change the title of holder.\n"
                                   "\n"
                                   "Put 10 to change an element in the table.\n"
                                   "Put 50 to print title.\n"
                                   "Put 51 to hide title.\n"
                                   "\n"
                                   "Put -1 to break.\n\n";

    const std::string cha_manual = "Put 1 to print all CONTENTS of the holder.\n"
                                   "Put 3 to change the title of holder.\n"
                                   "\n"
                                   "Put 10 to push a data.\n"
                                   "Put 11 to pop a data.\n"
                                   "Put 20 to modify the vertical size of the graph.\n"
                                   "Put 21 to modify the data name.\n"
                                   "Put 22 to modify the height.\n"
                                   "Put 50 to print title.\n"
                                   "Put 51 to hide title.\n"
                                   "\n"
                                   "Put -1 to break.\n\n";
}

#endif //OOPFINAL_MINT_UTILS_H
