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

    /**
     * size가 객체 생성 시에 정해지는 컨테이너
     * std::vector<T> 기반으로 구현됩니다.
     * @tparam T container 내부 원소의 타입
     */
    template<typename T>
    class StaticList {
        std::vector<T>      v;
    public:

        explicit            StaticList(size_t _size) : v(_size) {}
                            StaticList(size_t n, const T& _value) : v(n, _value) {}

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
    explicit                            Holder(std::string _title = "null_title");
    virtual                             ~Holder() = default;

    virtual void                        print() const       = 0;
    [[nodiscard]] virtual TYPE          get_type() const    = 0;
    [[nodiscard]] virtual std::string   to_txt_data() const = 0;

    [[nodiscard]] const std::string&    get_title() const;
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
    explicit StringHolder(const std::vector<std::string>& _data);
    StringHolder();

    // Print method
    void print() const override;

    // Get methods
    [[nodiscard]] TYPE get_type() const override;
    [[nodiscard]] int get_size() const;

    // Save method
    [[nodiscard]] std::string to_txt_data() const override;

private:
    [[nodiscard]] std::vector<std::pair<std::string, int>> data_split() const;

public:
    // Edit methods

    // remove : If data = "abcdefg", and we apply remove(3, 2), then we get edited data = "abcfg"
    void remove(unsigned int start, unsigned int length);

    // edit : If data = "abcdefg", and we apply edit(3, "str"), then we get editted data = "abcstrg"
    void edit(unsigned int start, const std::string& str);

    // insert : If data = "abcdefg", and we apply insert(3, "str"), then we get editted data = "abcstrdefg"
    void insert(unsigned int start, const std::string& str);

    // push : If data = "abc", and we apply push("defg"), then we get editted data = "abcdefg"
    void push(const std::string& str);

    // Spell-check method 2 : Improved spell-check with 2 tries
    void spellcheck(const Trie& trie1, const Trie& trie2, const int MAX_SUGGESTIONS = 1000);

};

#endif //OOPFINAL_HOLDERS_H