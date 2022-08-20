#include "holders.h"

template<typename T>
T &Holder::StaticList<T>::operator[](int idx) {
    return v[idx];
}

template<typename T>
T &Holder::StaticList<T>::operator[](unsigned int idx) {
    return v[idx];
}

template<typename T>
const T &Holder::StaticList<T>::operator[](int idx) const {
    return v[idx];
}

template<typename T>
const T &Holder::StaticList<T>::operator[](unsigned int idx) const {
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

/*
 * TableHolder::implements end
 */

ChartHolder::ChartHolder(const std::vector<std::string> &data)
        : Holder(data[0]), x_label_number(stoi(data[1])), i_pxl(stoi(data[2])) {
    show_title = true;

    for (int i = 0, j = 3; i < x_label_number; ++i) {
        const std::string& temp = data[j];
        ++j;
        const double temp2 = stod(data[j]);
        ++j;
        label.emplace_back(temp, temp2);
    }
}

ChartHolder::ChartHolder() : Holder(), x_label_number(1), i_pxl(7) {
    show_title = true;

    label.emplace_back("default", 10);
}

void ChartHolder::push(const std::string &str, double height) {
    label.emplace_back(str, height);
    ++x_label_number;
}

void ChartHolder::pop() {
    if (label.size() <= 1) {
        throw mints::unpoppable("The container is no more unpoppable");
    }
    label.pop_back();
    --x_label_number;
}

void ChartHolder::modify_dataname(const std::string &str, unsigned int idx) {
    if (idx >= label.size()) {
        throw mints::input_out_of_range("Index out of range, at ChartHolder::modify_dataname");
    }
    label[idx].data_name = str;
}

void ChartHolder::modify_height(double d, unsigned int idx) {
    if (idx >= label.size()) {
        throw mints::input_out_of_range("Index out of range, at ChartHolder::modify_height");
    }
    if (d <= 0) {
        throw mints::input_out_of_range("Height out of range, at ChartHolder::modify_height");
    }
    label[idx].height = d;
}

void ChartHolder::modify_i_size(unsigned int h) {
    i_pxl = h;
}

void ChartHolder::print() const {
    Layer frame(i_pxl, SubLayer(j_pxl(), ' '));

    this->set_axis(frame); // Draw x, y - axis
    this->set_data_name(frame); // Draw the data-name at our x - axis
    this->put_line(frame); // Draw pivots and line segments on our frame

    // Print
    std::cout << title << std::endl;
    for (int i = 0; i < i_pxl; ++i) {
        for (int j = 0; j < j_pxl(); ++j) {
            std::cout << frame[i][j];
        } std::cout << std::endl;
    } std::cout << std::endl;
}

unsigned int ChartHolder::j_pxl() const {
    return (max_length_of_data_name() + 1) * x_label_number + 2;
}

unsigned int ChartHolder::max_length_of_data_name() const {
    int max_len = 0;
    for (const Label& l : label) {
        int siz = (int) l.data_name.size();
        if (max_len < siz) {
            max_len = siz;
        }
    }
    return max_len;
}

void ChartHolder::set_axis(Holder::Layer &frame) const {
    for (int i = 0; i < i_pxl; ++i) {
        frame[i][1] = '|';
    }
    for (int j = 0; j < j_pxl(); ++j) {
        frame[i_pxl - 2][j] = '-';
    }
    frame[i_pxl - 2][1] = '+';
}

void ChartHolder::put_str_into_frame(Holder::Layer &frame, const std::string &str, unsigned int i_start,
                                     unsigned int j_start) const {
    for (int j = 0; j < str.size(); ++j) {
        if (j_start + j >= j_pxl()) {
            throw mints::input_out_of_range("");
        }
        frame[i_start][j_start + j] = str[j];
    }
}

void ChartHolder::set_data_name(Holder::Layer &frame) const {
    auto max_len = max_length_of_data_name();
    for (int j = 0; j < x_label_number; j++) {
        put_str_into_frame(frame, label[j].data_name, i_pxl - 1, 2 + j * (max_len + 1));
    }
}

double ChartHolder::get_max_height() const {
    double maxx = 0;
    for (const Label& l : label) {
        if (maxx < l.height) {
            maxx = l.height;
        }
    }
    return maxx;
}

std::vector<int> ChartHolder::standardize() const {
    std::vector<int> ret;
    ret.reserve(x_label_number);

    double max_height = get_max_height();
    unsigned int max_pixel = i_pxl - 2; // Since the last 2 pixels are allocated representing axes

    for (const Label& l : label) {
        int temp = static_cast<int>(round(l.height / max_height * max_pixel));
        ret.push_back(temp);
    }

    return ret;
}

/*
 * ChartHolder::implements end
 */

HistogramHolder::HistogramHolder(const std::vector<std::string> &data) : ChartHolder(data) {}

Holder::TYPE HistogramHolder::get_type() const {
    return HISTOGRAM_HOLDER;
}

std::string HistogramHolder::to_txt_data() const {
    std::stringstream ss;
    ss << "<HISTOGRAM>\n" << title << '\n' <<
       std::to_string(x_label_number) << '\n' << std::to_string(i_pxl) << '\n';
    for (auto& [dataname, height] : label) {
        ss << dataname << '\n' << height << '\n';
    } ss << "</HISTOGRAM>\n";

    std::string ret; ss >> ret;
    return ret;
}

void HistogramHolder::put_line(Holder::Layer &frame) const {
    // Set pivots
    std::vector<int> height_info = standardize();
    unsigned int max_len = max_length_of_data_name();
    for (int p = 0; p < x_label_number; ++p) {
        unsigned int pivot_i = i_pxl - 2 - height_info[p];
        unsigned int pivot_j = 2 + p * (max_len + 1) + max_len / 2;

        for (auto i = pivot_i; i < i_pxl - 2; ++i) {
            frame[i][pivot_j] = '#';
        }
    }
}

/*
 * HistogramHolder::implements end
 */

Holder::TYPE LineHolder::get_type() const {
    return LINE_HOLDER;
}

std::string LineHolder::to_txt_data() const {
    std::stringstream ss;
    ss << "<LINE>\n" << title << '\n' <<
       std::to_string(x_label_number) << '\n' << std::to_string(i_pxl) << '\n';

    for (auto& [dataname, height] : label) {
        ss << dataname << '\n' << height << '\n';
    } ss << "</LINE>\n";

    std::string ret; ss >> ret;
    return ret;
}

void LineHolder::put_segment(Holder::Layer &frame, int i1, int j1, int i2, int j2) {

    for (int j = j1 + 1; j < j2; ++j) {
        auto i = static_cast<int>(round((double)(i2 - i1) / (double)(j2 - j1) * (j - j1) + i1));
        frame[i][j] = '-';
    }
}

void LineHolder::put_line(Holder::Layer &frame) const {

    // Set pivots
    std::vector<int> height_info = standardize();
    auto max_len = max_length_of_data_name();
    for (int p = 0; p < x_label_number; ++p) {
        frame[i_pxl - 2 - height_info[p]][2 + p * (max_len + 1) + max_len / 2] = '#';
        /*
         i_pxl - 2 - height_info[p]:
         If i_pxl = 10 (i.e. max height we can use = 8), and height_info = 6, then it returns 8 - 6 = 2
         If i_pxl = 11 (i.e. max height we can use = 9), and height_info = 5, then it returns 9 - 5 = 4

         2 + p * (max_len + 1) + max_len / 2:
         2 + p * (max_len + 1) tells us the "first" j-pixel of each areas. To get the "middlepoint" of each areas, we add max_len + 2.
         f. e. if we have

         |    #*                         *#
         |      **                     **
         |        **                 **
         |          *#*           *#*
         |             ****   ****
         |                 *#*
         |
         |
         +------------------------------------
         | 2020-1 2020-2 2021-1 2021-2 2022-1

         then each area is divided:

         |    #  |      |      |      |   #
         |       |      |      |      |
         |       |      |      |      |
         |       |   #  |      |   #  |
         |       |      |      |      |
         |       |      |   #  |      |
         |       |      |      |      |
         |       |      |      |      |
         +------------------------------------
         | 2020-1 2020-2 2021-1 2021-2 2022-1

         then 2 + p * (max_len + 1) points:

           +      +      +      +      +
         |    #  |      |      |      |   #
         |       |      |      |      |
         |       |      |      |      |
         |       |   #  |      |   #  |
         |       |      |      |      |
         |       |      |   #  |      |
         |       |      |      |      |
         |       |      |      |      |
         +------------------------------------
         | 2020-1 2020-2 2021-1 2021-2 2022-1

         and 2 + p * (max_len + 1) + max_len / 2 points:

              +      +      +      +      +
         |    #  |      |      |      |   #
         |       |      |      |      |
         |       |      |      |      |
         |       |   #  |      |   #  |
         |       |      |      |      |
         |       |      |   #  |      |
         |       |      |      |      |
         |       |      |      |      |
         +------------------------------------
         | 2020-1 2020-2 2021-1 2021-2 2022-1

         */
    }

    // Set lines
    for (int p = 0; p < x_label_number - 1; ++p) {
        auto i1 = i_pxl - 2 - height_info[p];
        auto i2 = i_pxl - 2 - height_info[p + 1];
        auto j1 = 2 + p * (max_len + 1) + max_len / 2;
        auto j2 = 2 + (p + 1) * (max_len + 1) + max_len / 2;
        put_segment(frame, i1, j1, i2, j2);
    }
}
