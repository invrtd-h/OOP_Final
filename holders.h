#ifndef OOPFINAL_HOLDERS_H
#define OOPFINAL_HOLDERS_H

#include <iostream>
#include <sstream>
#include <utility>

#include "mint_utils.h"

class Holder {
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

#endif //OOPFINAL_HOLDERS_H