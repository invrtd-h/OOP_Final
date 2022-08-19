#include "holders.h"

template<typename T>
T &Holder::StaticList<T>::operator[](int idx) {
    return v[idx];
}

template<typename T>
const T &Holder::StaticList<T>::operator[](int idx) const {
    return v[idx];
}

template<typename T>
int Holder::StaticList<T>::size() const noexcept {
    return static_cast<int>(v.size());
}

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

/*
 * ShapeHolder::implements end
 */

TableHolder::TableHolder(const std::vector<std::string> &data) :
        Holder(data[0]), i_num(stoi(data[1])), j_num(stoi(data[2])) {
    show_title = false;

    content.resize(i_num, std::vector<std::string>(j_num));

    for (int i = 0, k = 3; i < i_num; ++i) {
        for (int j = 0; j < j_num; ++j) {
            content[i][j] = data[k++];
        }
    }
}

TableHolder::TableHolder(unsigned int _i, unsigned int _j) : Holder(), i_num(_i), j_num(_j) {
    show_title = false;

    content.resize(i_num, std::vector<std::string>(j_num));

    for (int i = 0; i < i_num; ++i) {
        for (int j = 0; j < j_num; ++j) {
            content[i][j] = "";
        }
    }
    content[0][0] = "default";
}

Holder::TYPE TableHolder::get_type() const {
    return TABLE_HOLDER;
}

std::string TableHolder::to_txt_data() const {
    std::stringstream ss;
    ss  << "<TABLE>\n" << title  << '\n' << std::to_string(i_num) << '\n'
        << std::to_string(j_num) << '\n';

    for (const auto& vs : content) {
        for (const auto& s : vs) {
            ss << s << '\n';
        }
    }

    ss << "</TABLE>\n";

    std::string ret; ss >> ret;
    return ret;
}

void TableHolder::put(const std::string &str, unsigned int _i, unsigned int _j) {
    if (_i >= i_num || _j >= j_num) {
        throw mints::input_out_of_range("Forbidden access request, at TableHolder::put");
    }
    content[_i][_j] = str;
}

void TableHolder::print() const {
    if (show_title) {
        std::cout << title << std::endl;
    }

    // Store the maximum length string of each column
    unsigned int block_size[j_num];
    for (int j = 0; j < j_num; ++j) {
        block_size[j] = maximum_len_thru_column(j);
    }

    unsigned int j_pxl_size = 0;
    for (int j = 0; j < j_num; ++j) {
        j_pxl_size += block_size[j];
    } j_pxl_size += j_num;

    for (int i = 0; i < i_num; ++i) {
        for (int j_pxl = 0; j_pxl < j_pxl_size; ++j_pxl) {
            std::cout << '-';
        }
        std::cout << std::endl;
        for (int j = 0; j < j_num; ++j) {
            std::cout << '|';
            print_block(content[i][j], block_size[j]);
        }
        std::cout << '\n';
    }
    for (int j_pxl = 0; j_pxl < j_pxl_size; ++j_pxl) {
        std::cout << '-';
    }
    std::cout << std::endl;
}

unsigned int TableHolder::maximum_len_thru_column(int j_idx) const {
    unsigned int max = 0;
    for (int i = 0; i < i_num; ++i) {
        if (max < content[i][j_idx].size()) {
            max = (unsigned int) content[i][j_idx].size();
        }
    }

    return max;
}

void TableHolder::print_block(const std::string &str, unsigned int block_size) {
    std::string empty_space;
    for (int i = 0; i < block_size - str.size(); ++i) {
        empty_space.push_back(' ');
    }
    std::cout << str << empty_space;
}
