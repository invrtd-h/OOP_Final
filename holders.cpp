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

/**
 * Holder 생성자
 * @param _title 제목
 */
Holder::Holder(std::string _title) : title(std::move(_title)), show_title(false) {}

/**
 * @return 제목
 */
const std::string& Holder::get_title() const {
    return title;
}

/**
 * @param str 새로운 제목
 */
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

/**
 * 기존 데이터에서 읽어온 정보를 기반으로 한 TableHolder 생성자
 * @param data 파싱된 데이터
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

/**
 * 새로운 TableHolder의 생성자
 * @param _i 행 개수
 * @param _j 열 개수
 */
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

/**
 * 문자열 삽입
 * @param str i행 j열에 삽입할 문자열
 * @param _i 행 번호
 * @param _j 열 번호
 */
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

/**
 * @example {"abc", "de", "fghij"} -> 5
 * @param j_idx 열 번호
 * @return 해당 열에서 가장 긴 원소의 길이
 */
unsigned int TableHolder::maximum_len_thru_column(int j_idx) const {
    unsigned int max = 0;
    for (int i = 0; i < i_num; ++i) {
        if (max < content[i][j_idx].size()) {
            max = (unsigned int) content[i][j_idx].size();
        }
    }

    return max;
}

/**
 * 주어진 사이즈에 맞춰 문자열을 출력하는 함수
 * @example ("abcde", 8) -> // print "abcde   "
 * @param str 프린트할 문자열
 * @param block_size 프린트할 블록 사이즈
 */
void TableHolder::print_block(const std::string &str, unsigned int block_size) {
    std::string empty_space(block_size - str.size(), ' ');
    std::cout << str << empty_space;
}

/*
 * TableHolder::implements end
 */

ChartHolder::ChartHolder(const std::vector<std::string> &data)
        : Holder(data[0])
        , x_label_number(stoi(data[1]))
        , i_pxl(stoi(data[2])) {
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

/*
 * LineHolder::implementations end
 */

StringHolder::StringHolder(const std::vector<std::string> &_data) : Holder(_data[0]) {
    show_title = false;

    std::string hold;
    for (int i = 1; i < _data.size(); ++i) {
        const std::string& str = _data[i];
        hold += str;
        hold += '\n';
    }
    data = hold;
}

StringHolder::StringHolder() : Holder() {
    show_title = false;
}

void StringHolder::print() const {
    if (show_title) {
        std::cout << title << std::endl;
    }
    if (data.empty()) {
        std::cout << "(EMPTY HOLDER)" << std::endl;
    } else {
        std::cout << data << std::endl;
    }
}

Holder::TYPE StringHolder::get_type() const {
    return STRING_HOLDER;
}

int StringHolder::get_size() const {
    return (int) data.size();
}

std::string StringHolder::to_txt_data() const {
    std::stringstream ss;
    ss << "<STRINGHOLDER>\n" << title << '\n' << data << '\n' << "</STRINGHOLDER>\n";

    std::string ret; ss >> ret;
    return ret;
}

std::vector<std::pair<std::string, int>> StringHolder::data_split() const {
    std::vector<std::pair<std::string, int>> vecpair;
    vecpair.emplace_back("", 0);

    for (int i = 0; i < data.size(); ++i) {
        if (65 <= data[i] && data[i] <= 90) {
            vecpair.back().first += data[i] + 32;
        } else if (97 <= data[i] && data[i] <= 122) {
            vecpair.back().first += data[i];
        } else if (!vecpair.back().first.empty()) {
            vecpair.emplace_back("", i + 1);
        } else {
            ++vecpair.back().second;
        }
    }

    while (vecpair.back().first.empty()) {
        vecpair.pop_back();
    }

    return vecpair;
}

void StringHolder::remove(unsigned int start, unsigned int length) {
    if (data.size() <= start) {
        return;
    }
    if (start + length >= data.size()) {
        data = data.substr(0, start);
    } else {
        data = data.substr(0, start) + data.substr(start + length);
    }
}

void StringHolder::edit(unsigned int start, const std::string &str) {
    int idx = (int) str.size() < start ? (int) str.size() : start;
    if (idx + str.size() >= data.size()) {
        data = data.substr(0, idx) + str;
    } else {
        data = data.substr(0, idx) + str + data.substr(idx + str.size());
    }
}

void StringHolder::insert(unsigned int start, const std::string &str) {
    int idx = (int) data.size() < start ? (int) data.size() : start;
    data = data.substr(0, idx) + str + data.substr(idx);
}

void StringHolder::push(const std::string &str) {
    data += str;
}

void StringHolder::spellcheck(const Trie &trie1, const Trie &trie2, const int MAX_SUGGESTIONS) {
    std::vector<std::pair<std::string, int>> vecpair = data_split();

    // If the length of data is modified by our spell-check operation, then our mlsf variable will revise it
    int modified_length_so_far = 0;
    int& mlsf = modified_length_so_far; // This reference is just an abbreviation

    unsigned int idx = -1;

    for (const auto& pairpair : vecpair) {
        std::string str = mints::make_lowercase(pairpair.first);
        if (trie1._contains_(str)) {
            // If our letter is in out trie, i.e. right spell, then just pass
            continue;
        }
        // Get our suggests list in our Trie
        std::vector<std::string> suggests = trie1.get_suggestions(str, MAX_SUGGESTIONS * 20);
        std::vector<std::string> hold = trie2.get_suggestions(str, MAX_SUGGESTIONS * 20);
        suggests.insert(suggests.end(), hold.begin(), hold.end());

        // Sort in increasing order of closeness
        // Since sorting the strings directly causes a lot of memory allocations, we rather sort the pointer of strings, not strings
        std::string* ptrs[suggests.size()];
        for (int i = 0; i < suggests.size(); ++i) {
            ptrs[i] = &suggests[i];
        }

        const std::function<std::string(const std::string&)> read_dir_1 = trie1.get_preprocess();
        const std::function<std::string(const std::string&)> read_dir_2 = trie2.get_preprocess();

        // The lambda function in the 3rd component of 'sort' is a function that reads two string pointers (lhs, rhs) and return whether *lhs is closer to 'str' than *rhs.
        std::sort(ptrs, ptrs + suggests.size(),
                  [str, &read_dir_1, &read_dir_2](std::string* lhs, std::string* rhs) -> bool {
                      return mints::closeness(*lhs, str, read_dir_1) + mints::closeness(*lhs, str, read_dir_2) > mints::closeness(*rhs, str, read_dir_1) + mints::closeness(*rhs, str, read_dir_2);
                  });

        // If MAX_SUGGESTIONS = 20 and suggests.size() = 15, then we suggest 15 words
        // If MAX_SUGGESTIONS = 20 and suggests.size() = 40, then we suggest 20 words
        const int final_recommending_number = suggests.size() < MAX_SUGGESTIONS ? (int) suggests.size() : MAX_SUGGESTIONS;

        // Print our alternative words
        std::cout << "The alternative words for word '" << str << "' are: " << std::endl;
        for (int i = 0; i < final_recommending_number; ++i) {
            std::cout << i + 1 << " " << *ptrs[i] << " | ";
        } std::cout << std::endl;

        // Asks users that do you want to correct it
        do {
            std::cout << "Put a number if you want to correct (Put 0 if you don't want) : ";
            std::cin >> idx;
            std::cin.ignore(1000, '\n');
        } while (idx > final_recommending_number);
        --idx;

        if (idx == -1) {
            continue;
        } else {
            remove(pairpair.second + mlsf, (unsigned int) pairpair.first.size());
            insert(pairpair.second + mlsf, *ptrs[idx]);

            // mlsf -= size to delete; mlsf += size to put;
            mlsf -= (unsigned int) pairpair.first.size() - ptrs[idx]->size();

            print();
        }

        std::cout << std::endl;

    }

    std::cout << "END OF SPELL-CHECK" << std::endl << std::endl;

}
