#include "holders.h"

/*
 * Holder::implements
 */

Holder::Holder(std::string t) : title(std::move(t)), show_title(false) {}

std::string Holder::get_title() const {
    return title;
}

void Holder::set_title(const std::string &str) {
    title = str;
}

void Holder::title_on() {
    show_title = true;
}

void Holder::title_off() {
    show_title = false;
}

/*
 * Holder::implements end
 */

TestHolder::TestHolder(std::string _data) : data(std::move(_data)) {}

TestHolder::TestHolder(const std::vector<std::string> &_data) : Holder() {
    std::stringstream ss;
    for (const auto& s : _data) {
        ss << s << '\n';
    }
    ss >> data;
}

void TestHolder::print() const {
    if (show_title) {
        std::cout << title << std::endl;
    }
    std::cout << data << std::endl;
}

Holder::TYPE TestHolder::get_type() const {
    return TEST_HOLDER;
}

std::string TestHolder::to_txt_data() const {
    return "<TESTHOLDER>\n" + data + "</TESTHOLDER>\n";
}

/*
 * TestHolder::implements end
 */

ShapeHolder::ShapeHolder(const std::vector<std::string> &data) : Holder(), j_pxl(stoi(data[0])), i_pxl(stoi(data[1])) {}

Holder::TYPE ShapeHolder::get_type() const {
    return SHAPE_HOLDER;
}

std::string ShapeHolder::to_txt_data() const {
    std::stringstream ss;
    ss << "<SHAPE_HOLDER>\n" << std::to_string(j_pxl) << "\n" << std::to_string(i_pxl) << "\n</SHAPE_HOLDER>\n";

    std::string ret; ss >> ret;
    return ret;
}
