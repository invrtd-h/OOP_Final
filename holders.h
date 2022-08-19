#ifndef OOPFINAL_HOLDERS_H
#define OOPFINAL_HOLDERS_H

#include <iostream>
#include <sstream>
#include <utility>

#include "mint_utils.h"

class Holder {
protected:
    template<typename T>
    class StaticList {
        std::vector<T>      v;
    public:
        explicit            StaticList(size_t n) : v(n) {}
                            StaticList(size_t n, const T& t) : v(n, t) {}

        T&                  operator[](int idx);
        const T&            operator[](int idx) const;
        [[nodiscard]] int   size() const noexcept;
    };

    using SubLayer  = StaticList<char>;
    using Layer     = StaticList<SubLayer>;

public:
    enum TYPE {HOLDER, TEST_HOLDER, SHAPE_HOLDER, STRING_HOLDER, TABLE_HOLDER, LINE_HOLDER, HISTOGRAM_HOLDER};

protected:
    std::string                         title;
    bool                                show_title;

public:
    explicit                            Holder(std::string t = "null_title");
    virtual                             ~Holder() = default;

    virtual void                        print() const       = 0;
    [[nodiscard]] virtual TYPE          get_type() const    = 0;
    [[nodiscard]] virtual std::string   to_txt_data() const = 0;

    [[nodiscard]] std::string           get_title() const;
    void                                set_title(const std::string& str);

    void                                title_on();
    void                                title_off();

};

class TestHolder : public Holder {
protected:
    std::string data;
public:
    TestHolder() = default;
    explicit                    TestHolder(std::string _data);
    explicit                    TestHolder(const std::vector<std::string>& _data);

    void                        print() const override;
    [[nodiscard]] TYPE          get_type() const override;
    [[nodiscard]] std::string   to_txt_data() const override;
};

class ShapeHolder : public Holder {
private:
    int j_pxl, i_pxl;
public:
    explicit                    ShapeHolder(const std::vector<std::string>& data);
    [[nodiscard]] TYPE          get_type() const override;
    [[nodiscard]] std::string   to_txt_data() const override;

    // TODO : IMPLEMENT
    void print() const override {

    }
};

class TableHolder : public Holder {
    std::vector<std::vector<std::string>>       content;
    unsigned int                                i_num, j_num;

public:
    explicit                                    TableHolder(const std::vector<std::string>& data);
    explicit                                    TableHolder(unsigned int _i = 1, unsigned int _j = 1);

    [[nodiscard]] TYPE                          get_type() const override;
    [[nodiscard]] std::string                   to_txt_data() const override;

    void                                        put(const std::string& str, unsigned int _i, unsigned int _j);
    void                                        print() const override;

private:
    [[nodiscard]] unsigned int                  maximum_len_thru_column(int j_idx) const;
    static void                                 print_block(const std::string& str, unsigned int block_size);
};

#endif //OOPFINAL_HOLDERS_H