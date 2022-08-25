#ifndef OOPFINAL_LISTENER_H
#define OOPFINAL_LISTENER_H

#include "docus.h"

class Listener {
    Document* doc_ptr;
    Trie* trie_ptr1;
    Trie* trie_ptr2;
    int how_many_words_do_you_want;

public:

    Listener(const std::vector<std::string>& sd, const std::vector<std::string>& strd);

    ~Listener();

    std::string listen();

private:

    static void print_manual() {
        std::cout << mint_manuals::general_manual;
    }
    static void print_str_manual() {
        std::cout << mint_manuals::str_manual;
    }
    static void print_tab_manual() {
        std::cout << mint_manuals::tab_manual;
    }
    static void print_cha_manual() {
        std::cout << mint_manuals::cha_manual;
    }

    // Access functions
    void access(int n);

    void modify_str(StringHolder* p);
    static void modify_tab(TableHolder* p);
    static void modify_cha(ChartHolder* p);

    std::string save() {
        return doc_ptr->save();
    }
};

#endif //OOPFINAL_LISTENER_H
