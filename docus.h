#ifndef OOPFINAL_DOCUS_H
#define OOPFINAL_DOCUS_H

#include <vector>
#include "holders.h"

class Document {
private:
    std::vector<Holder*>        holders;
    std::string                 filename;
public:
    explicit                    Document(const std::vector<std::string>& data);
                                ~Document();

    void                        print() const;
    void                        print_infos() const;

    void                        push_tes();
    void                        push_str();
    void                        push_tab(int i = 1, int j = 1);
    void                        push_lin();
    void                        push_his();

    void                        push_tes_infrontof(int idx);
    void                        push_str_infrontof(int idx);
    void                        push_tab_infrontof(int idx, int i = 1, int j = 1);
    void                        push_lin_infrontof(int idx);
    void                        push_his_infrontof(int idx);

    void                        pop_holder();
    void                        remove_holder(int idx);

    [[nodiscard]] std::string   save() const;

    Holder*                     at(int idx);

    [[nodiscard]] unsigned int  size() const;
    [[nodiscard]] std::string   get_filename() const;
};

#endif //OOPFINAL_DOCUS_H
