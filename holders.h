#ifndef OOPFINAL_HOLDERS_H
#define OOPFINAL_HOLDERS_H

#include <iostream>
#include <sstream>
#include <utility>
#include <cmath>

#include "mint_utils.h"
#include "trie.h"

class Holder {
protected:
    template<typename T>
    class StaticList {
        std::vector<T>      v;
    public:
        /**
         *
         * @param _size
         */
        explicit            StaticList(size_t _size) : v(_size) {}
                            StaticList(size_t n, const T& t) : v(n, t) {}

        T&                  operator[](int idx);
        T&                  operator[](unsigned int idx);
        const T&            operator[](int idx) const;
        const T&            operator[](unsigned int idx) const;
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

class ChartHolder : public Holder {

protected:

    struct Label {
        std::string data_name;
        double height;
        Label(std::string _x, double _height) : data_name(std::move(_x)), height(_height) {}
    };

    unsigned int                    i_pxl, x_label_number;
    std::vector<Label>              label;

public:
    explicit                        ChartHolder(const std::vector<std::string>& data);
                                    ChartHolder();

    // Modify
    void                            push(const std::string& str, double height);
    void                            pop();
    void                            modify_dataname(const std::string& str, unsigned int idx);
    void                            modify_height(double d, unsigned int idx);
    void                            modify_i_size(unsigned int h);

    void                            print() const override;

protected:

    virtual void                    put_line(Layer& frame) const = 0;

    [[nodiscard]] unsigned int      j_pxl() const;

    // the longest data name in our data
    [[nodiscard]] unsigned int      max_length_of_data_name() const;

    // Draw x, y - axis
    void                            set_axis(Layer& frame) const;

    // put the string we want at any point of our frame.
    /*
     If the frame is given
     [     ]
     [     ]
     [     ]
     [     ] (i = 4, j = 5)
     then if we operate put_str_into_frame(frame, "we", 2, 2) then we get
     [     ]
     [     ]
     [  we ]
     [     ] (i = 4, j = 5)
     */
    void                            put_str_into_frame
    (Layer& frame, const std::string& str, unsigned int i_start, unsigned int j_start) const;

    // Draw the data-name at our x - axis
    void                            set_data_name(Layer& frame) const;

    // Get the maximum value of our data; We use this when we standardize our data (See standardize().)
    // If our data is {Label{"AAA", 2}, Label{"BBB", 12}, Label{"CCC", 10}}, then we get 12
    [[nodiscard]] double            get_max_height() const;

    // If we have a data of height 100, but only 10 pixels are allocated, then we cannot print the given data;
    // So we use standardize() function. If we have {10, 20, 40} and our allocated height is 8, then it will return {2, 4, 8}.
    [[nodiscard]] std::vector<int>  standardize() const;
};


class HistogramHolder : public ChartHolder {
    /*
     GraphExample
     |    #
     |    #
     |    #
     |    #      #
     |    #      #
     |    #      #      #
     |    #      #      #
     |    #      #      #
     +----------------------
     | 2020-1 2020-2 2021-1
     */

public:
    explicit                    HistogramHolder(const std::vector<std::string>& data);
                                HistogramHolder() : ChartHolder() {}

    [[nodiscard]] TYPE          get_type() const override;

    // Save method
    [[nodiscard]] std::string   to_txt_data() const override;

    void                        put_line(Layer& frame) const override;
};

class LineHolder : public ChartHolder {

    /*
     GraphExample
     |    #
     |     *
     |      **
     |        **
     |          *
     |           #***       ***#***
     |               ***#***       ***#
     +------------------------------------
     | 2020-1 2020-2 2021-1 2021-2 2022-1
     */

public:
    explicit                    LineHolder(const std::vector<std::string>& data) : ChartHolder(data) {}
                                LineHolder() : ChartHolder() {}

    [[nodiscard]] TYPE          get_type() const override;
    [[nodiscard]] std::string   to_txt_data() const override;

private:
    // This draws a line between the point (i1, j1) and (i2, j2)
    static void                 put_segment(Layer& frame, int32_t i1, int32_t j1, int32_t i2, int32_t j2);
    void                        put_line(Layer& frame) const override;
};

class StringHolder : public Holder {
    std::string data;
public:
    explicit StringHolder(const std::vector<std::string>& _data) : Holder(_data[0]) {
        show_title = false;

        std::string hold;
        for (int i = 1; i < _data.size(); ++i) {
            const std::string& str = _data[i];
            hold += str;
            hold += '\n';
        }
        data = hold;
    }
    StringHolder() : Holder() {
        show_title = false;
    }

    // Print method
    void print() const override {
        if (show_title) {
            std::cout << title << std::endl;
        }
        if (data.empty()) {
            std::cout << "(EMPTY HOLDER)" << std::endl;
        } else {
            std::cout << data << std::endl;
        }
    }

    // Get methods
    [[nodiscard]] TYPE get_type() const override {
        return STRING_HOLDER;
    }
    [[nodiscard]] int get_size() const {
        return (int) data.size();
    }

    // Save method
    [[nodiscard]] std::string to_txt_data() const override {
        std::stringstream ss;
        ss << "<STRINGHOLDER>\n" << title << '\n' << data << '\n' << "</STRINGHOLDER>\n";

        std::string ret; ss >> ret;
        return ret;
    }

private:
    [[nodiscard]] std::vector<std::pair<std::string, int>> data_split() const {
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

public:
    // Edit methods

    // remove : If data = "abcdefg", and we apply remove(3, 2), then we get editted data = "abcfg"
    void remove(unsigned int start, unsigned int length) {
        if (data.size() <= start) {
            return;
        }
        if (start + length >= data.size()) {
            data = data.substr(0, start);
        } else {
            data = data.substr(0, start) + data.substr(start + length);
        }
    }

    // edit : If data = "abcdefg", and we apply edit(3, "str"), then we get editted data = "abcstrg"
    void edit(unsigned int start, const std::string& str) {
        int idx = (int) str.size() < start ? (int) str.size() : start;
        if (idx + str.size() >= data.size()) {
            data = data.substr(0, idx) + str;
        } else {
            data = data.substr(0, idx) + str + data.substr(idx + str.size());
        }
    }

    // insert : If data = "abcdefg", and we apply insert(3, "str"), then we get editted data = "abcstrdefg"
    void insert(unsigned int start, const std::string& str) {
        int idx = (int) data.size() < start ? (int) data.size() : start;
        data = data.substr(0, idx) + str + data.substr(idx);
    }

    // push : If data = "abc", and we apply push("defg"), then we get editted data = "abcdefg"
    void push(const std::string& str) {
        data += str;
    }

    // Spell-check method 2 : Improved spell-check with 2 tries
    void spellcheck(const Trie& trie1, const Trie& trie2, const int MAX_SUGGESTIONS = 1000) {
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

};

#endif //OOPFINAL_HOLDERS_H