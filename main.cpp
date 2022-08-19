#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <vector>
#include <cmath>

class Node;
class Trie;
class Clipboard;
class Document;
class Holder;
class TestHolder;
class StringHolder;
class ShapeHolder;
class TableHolder;
class LineHolder;
class Listener;

#include "mint_utils.h"
#include "holders.h"
#include "trie.h"

using namespace std;

enum ERROR_INFO {OUT_OF_RANGE_ERROR, UNPOPPABLE_CONTAINER_ERROR, TOO_LARGE_ARGUMENT_ERROR, NIL};

struct ERRORS {
    ERROR_INFO error_info;
    string error_message;
    ERRORS(ERROR_INFO e, string s) : error_info(e), error_message(std::move(s)) {}
};

class Clipboard {
private:
    string arr[100];
    string error_message;

public:
    Clipboard() : error_message("(err}{err)") {}

    // Copy a string into a clipboard
    void push_into(const string& str, unsigned int n) {
        if (n < 100) {
            arr[n] = str;
        }
    }

    // Bring out a string to paste it
    [[nodiscard]] const string& get(unsigned int n) const {
        if (n < 100) {
            return arr[n];
        } else {
            return error_message;
        }
    }
};

class StringHolder : public Holder {

private:
    string data;

public:
    explicit StringHolder(const vector<string>& _data) : Holder(_data[0]) {
        show_title = false;

        string hold;
        for (int i = 1; i < _data.size(); ++i) {
            const string& str = _data[i];
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
            cout << title << endl;
        }
        if (data.empty()) {
            cout << "(EMPTY HOLDER)" << endl;
        } else {
            cout << data << endl;
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
    [[nodiscard]] vector<pair<string, int>> data_split() const {
        vector<pair<string, int>> vecpair;
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
    void edit(unsigned int start, const string& str) {
        int idx = (int) str.size() < start ? (int) str.size() : start;
        if (idx + str.size() >= data.size()) {
            data = data.substr(0, idx) + str;
        } else {
            data = data.substr(0, idx) + str + data.substr(idx + str.size());
        }
    }

    // insert : If data = "abcdefg", and we apply insert(3, "str"), then we get editted data = "abcstrdefg"
    void insert(unsigned int start, const string& str) {
        int idx = (int) data.size() < start ? (int) data.size() : start;
        data = data.substr(0, idx) + str + data.substr(idx);
    }

    // push : If data = "abc", and we apply push("defg"), then we get editted data = "abcdefg"
    void push(const string& str) {
        data += str;
    }

    // Copy & Paste method
    void copy(unsigned int start, unsigned int length, unsigned int n, Clipboard& clipboard) const {
        if (n >= 100) {
            cout << "Wrong copy index." << endl;
            return;
        }
        if (start >= data.size()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at StringHolder::copy");
            return;
        }
        if (start + length >= data.size()) {
            clipboard.push_into(data.substr(start), n);
        } else {
            clipboard.push_into(data.substr(start, length), n);
        }
    }

    void paste(unsigned int start, unsigned int n, Clipboard& clipboard) {
        int idx = (int) data.size() < start ? (int) data.size() : start;
        if (n >= 100) {
            return;
        }
        data = data.substr(0, idx) + clipboard.get(n) + data.substr(idx);
    }

    // Spell-check method
    void spellcheck(const Trie& trie, const int MAX_SUGGESTIONS = 12) {
        vector<pair<string, int>> vecpair = data_split();

        // If the length of data is modified by our spell-check operation, then our mlsf variable will revise it
        int modified_length_so_far = 0;
        int& mlsf = modified_length_so_far; // This reference is just an abbreviation

        unsigned int idx = -1;

        for (const pair<string, int>& pairpair : vecpair) {
            string str = mints::make_lowercase(pairpair.first);
            if (!trie._contains_(str)) {
                // Get our suggests list in our Trie
                vector<string> suggests = trie.get_suggestions(str, MAX_SUGGESTIONS);

                // Print our alternative words
                cout << "The alternative words for word '" << str << "' are: " << endl;
                for (int i = 0; i < suggests.size(); ++i) {
                    cout << i + 1 << " " << suggests[i] << " | ";
                } cout << endl;

                // Asks users that do you want to correct it
                do {
                    cout << "Put a number if you want to correct (Put 0 if you don't want) : ";
                    cin >> idx;
                    cin.ignore(1000, '\n');
                } while (idx > suggests.size());
                --idx;

                if (idx + 1 == 0) {
                    continue;
                } else {
                    remove(pairpair.second + mlsf, (unsigned int) pairpair.first.size());
                    insert(pairpair.second + mlsf, suggests[idx]);

                    // mlsf -= size to delete; mlsf += size to put;
                    mlsf -= (unsigned int) pairpair.first.size() - suggests[idx].size();
                }

                cout << endl;
            }
        }

        cout << "END OF SPELL-CHECK" << endl << endl;

    }

    // Spell-check method 2 : Improved spell-check with 2 tries
    void spellcheck(const Trie& trie1, const Trie& trie2, const int MAX_SUGGESTIONS = 1000) {
        vector<pair<string, int>> vecpair = data_split();

        // If the length of data is modified by our spell-check operation, then our mlsf variable will revise it
        int modified_length_so_far = 0;
        int& mlsf = modified_length_so_far; // This reference is just an abbreviation

        unsigned int idx = -1;

        for (const pair<string, int>& pairpair : vecpair) {
            string str = mints::make_lowercase(pairpair.first);
            if (trie1._contains_(str)) {
                // If our letter is in out trie, i.e. right spell, then just pass
                continue;
            }
            // Get our suggests list in our Trie
            vector<string> suggests = trie1.get_suggestions(str, MAX_SUGGESTIONS * 20);
            vector<string> hold = trie2.get_suggestions(str, MAX_SUGGESTIONS * 20);
            suggests.insert(suggests.end(), hold.begin(), hold.end());

            // Sort in increasing order of closeness
            // Since sorting the strings directly causes a lot of memory allocations, we rather sort the pointer of strings, not strings
            string* ptrs[suggests.size()];
            for (int i = 0; i < suggests.size(); ++i) {
                ptrs[i] = &suggests[i];
            }

            const function<string(const string&)> read_dir_1 = trie1.get_preprocess();
            const function<string(const string&)> read_dir_2 = trie2.get_preprocess();

            // The lambda function in the 3rd component of 'sort' is a function that reads two string pointers (lhs, rhs) and return whether *lhs is closer to 'str' than *rhs.
            sort(ptrs, ptrs + suggests.size(), [str, &read_dir_1, &read_dir_2](string* lhs, string* rhs) -> bool {
                return mints::closeness(*lhs, str, read_dir_1) + mints::closeness(*lhs, str, read_dir_2) > mints::closeness(*rhs, str, read_dir_1) + mints::closeness(*rhs, str, read_dir_2);
            });

            // If MAX_SUGGESTIONS = 20 and suggests.size() = 15, then we suggest 15 words
            // If MAX_SUGGESTIONS = 20 and suggests.size() = 40, then we suggest 20 words
            const int final_recommending_number = suggests.size() < MAX_SUGGESTIONS ? (int) suggests.size() : MAX_SUGGESTIONS;

            // Print our alternative words
            cout << "The alternative words for word '" << str << "' are: " << endl;
            for (int i = 0; i < final_recommending_number; ++i) {
                cout << i + 1 << " " << *ptrs[i] << " | ";
            } cout << endl;

            // Asks users that do you want to correct it
            do {
                cout << "Put a number if you want to correct (Put 0 if you don't want) : ";
                cin >> idx;
                cin.ignore(1000, '\n');
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

            cout << endl;

        }

        cout << "END OF SPELL-CHECK" << endl << endl;

    }

};

class ChartHolder : public Holder {

protected:

    class SubLayer {
    private:
        char* vc;
        int j_pxl;

    public:
        SubLayer() : j_pxl(0), vc(nullptr) {
            // Do nothing
        }
        ~SubLayer() {
            if (vc != nullptr) {
                // If something is allocated at vc, then delete it
                delete[] vc;
            }
        }

        SubLayer(const SubLayer&) = delete;
        SubLayer(const SubLayer&&) = delete;
        SubLayer& operator=(const SubLayer&) = delete;

        void allocate(int _j) {
            j_pxl = _j;

            vc = new char[_j];
            for (int j = 0; j < _j; ++j) {
                vc[j] = ' ';
            }
        }

        char& operator[](int idx) {
            return vc[idx];
        }

        char operator[](int idx) const {
            return vc[idx];
        }
    };

    class Layer {
    private:
        SubLayer* vvc;
        int i_pxl, j_pxl;

    public:
        Layer(int _i, int _j) : i_pxl(_i), j_pxl(_j) {
            if (_i <= 0 || _j <= 0 || _i > 10000 || _j > 10000) {
                throw mints::too_large_argument("required too large size of layer, at Layer::Layer");
            }
            vvc = new SubLayer[_i];
            for (int i = 0; i < _i; ++i) {
                vvc[i].allocate(_j);
            }
        }
        ~Layer() {
            delete[] vvc;
        }

        Layer(const Layer&) = delete;
        Layer(const Layer&&) = delete;
        Layer& operator=(const Layer&) = delete;

        SubLayer& operator[](int idx) {
            return vvc[idx];
        }

        const SubLayer& operator[](int idx) const {
            return vvc[idx];
        }
    };

    struct Label {
        string data_name;
        double height;
        Label(string _x, double _height) : data_name(std::move(_x)), height(_height) {}
    };

    int i_pxl, x_label_number;
    vector<Label> label;

public:
    explicit ChartHolder(const vector<string>& data) : Holder(data[0]), x_label_number(stoi(data[1])), i_pxl(stoi(data[2])) {
        show_title = true;

        int j = 3;
        for (int i = 0; i < x_label_number; ++i) {
            const string& temp = data[j];
            ++j;
            const double temp2 = stod(data[j]);
            ++j;
            label.emplace_back(temp, temp2);
        }
    }
    ChartHolder() : Holder(), x_label_number(1), i_pxl(7) {
        show_title = true;

        label.emplace_back("default", 10);
    }

    // Modify
    void push(const string& str, double height) {
        label.emplace_back(str, height);
        ++x_label_number;
    }
    void pop() {
        if (label.size() <= 1) {
            throw ERRORS(UNPOPPABLE_CONTAINER_ERROR, "occured at ChartHolder::pop");
        }
        label.pop_back();
        --x_label_number;
    }
    void modify_dataname(const string& str, unsigned int idx) {
        if (idx >= label.size()) {
            cout << "Invalid index" << endl;
            return;
        }
        label[idx].data_name = str;
    }
    void modify_height(double d, unsigned int idx) {
        if (idx >= label.size()) {
            cout << "Invalid index" << endl;
            return;
        }
        if (d <= 0) {
            cout << "Invalid number : The input should be a positive number" << endl;
            return;
        }
        label[idx].height = d;
    }
    void modify_i_size(unsigned int h) {
        i_pxl = h;
    }

    void print() const override {
        Layer frame(i_pxl, j_pxl());

        this->set_axis(frame); // Draw x, y - axis
        this->set_data_name(frame); // Draw the data-name at our x - axis
        this->put_line(frame); // Draw pivots and line segments on our frame

        // Print
        cout << title << endl;
        for (int i = 0; i < i_pxl; ++i) {
            for (int j = 0; j < j_pxl(); ++j) {
                cout << frame[i][j];
            } cout << endl;
        } cout << endl;
    }

protected:

    virtual void put_line(Layer& frame) const = 0;

    [[nodiscard]] unsigned int j_pxl() const {
        return (max_length_of_data_name() + 1) * x_label_number + 2;
    }

    // the longest data name in our data
    [[nodiscard]] unsigned int max_length_of_data_name() const {
        int max_len = 0;
        for (const Label& l : label) {
            int siz = (int) l.data_name.size();
            if (max_len < siz) {
                max_len = siz;
            }
        }
        return max_len;
    }

    // Draw x, y - axis
    void set_axis(Layer& frame) const {
        for (int i = 0; i < i_pxl; ++i) {
            frame[i][1] = '|';
        }
        for (int j = 0; j < j_pxl(); ++j) {
            frame[i_pxl - 2][j] = '-';
        }
        frame[i_pxl - 2][1] = '+';
    }

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
    void put_str_into_frame(Layer& frame, const string& str, int i_start, int j_start) const {
        for (int j = 0; j < str.size(); ++j) {
            if (j_start + j >= j_pxl()) {
                throw ERRORS(OUT_OF_RANGE_ERROR, "at LineHolder::put_str_into_frame");
                return;
            }
            frame[i_start][j_start + j] = str[j];
        }
    }
    // Draw the data-name at our x - axis
    void set_data_name(Layer& frame) const {
        auto max_len = max_length_of_data_name();
        for (int j = 0; j < x_label_number; j++) {
            put_str_into_frame(frame, label[j].data_name, i_pxl - 1, 2 + j * (max_len + 1));
        }
    }

    // Get the maximum value of our data; We use this when we standardize our data (See standardize().)
    // If our data is {Label{"AAA", 2}, Label{"BBB", 12}, Label{"CCC", 10}}, then we get 12
    [[nodiscard]] double get_max_height() const {
        double maxx = 0;
        for (const Label& l : label) {
            if (maxx < l.height) {
                maxx = l.height;
            }
        }
        return maxx;
    }
    // If we have a data of height 100, but only 10 pixels are allocated, then we cannot print the given data;
    // So we use standardize() function. If we have {10, 20, 40} and our allocated height is 8, then it will return {2, 4, 8}.
    [[nodiscard]] vector<int> standardize() const {
        vector<int> ret;
        ret.reserve(x_label_number);

        double max_height = get_max_height();
        int max_pixel = i_pxl - 2; // Since the last 2 pixels are allocated representing axes

        for (const Label& l : label) {
            int temp = static_cast<int>(round(l.height / max_height * max_pixel));
            ret.push_back(temp);
        }

        return ret;
    }
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
    explicit HistogramHolder(const vector<string>& data) : ChartHolder(data) {}
    HistogramHolder() : ChartHolder() {}

    [[nodiscard]] TYPE get_type() const override {
        return HISTOGRAM_HOLDER;
    }

    // Save method
    [[nodiscard]] std::string to_txt_data() const override {
        std::stringstream ss;
        ss << "<HISTOGRAM>\n" << title << '\n' <<
        std::to_string(x_label_number) << '\n' << std::to_string(i_pxl) << '\n';
        for (auto& [dataname, height] : label) {
            ss << dataname << '\n' << height << '\n';
        } ss << "</HISTOGRAM>\n";

        std::string ret; ss >> ret;
        return ret;
    }

    void put_line(Layer& frame) const override {
        // Set pivots
        vector<int> height_info = standardize();
        unsigned int max_len = max_length_of_data_name();
        for (int p = 0; p < x_label_number; ++p) {
            unsigned int pivot_i = i_pxl - 2 - height_info[p];
            unsigned int pivot_j = 2 + p * (max_len + 1) + max_len / 2;

            for (auto i = pivot_i; i < i_pxl - 2; ++i) {
                frame[i][pivot_j] = '#';
            }
        }
    }
};

class LineHolder : public ChartHolder {

    /*
     Here, we make a new class named 'SubLayer' and 'Layer' class. It will function like char** pointer which points 2-dim char-type array, but our 'Layer' class has a benefit that we can keep RAII rule to program securely. Also, we don't want to make our 'Layer' size be flexible, so we didn't use vector<vector<char>> and we implemented a new class.
     */

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
    explicit LineHolder(const vector<string>& data) : ChartHolder(data) {}
    LineHolder() : ChartHolder() {}

    [[nodiscard]] TYPE get_type() const override {
        return LINE_HOLDER;
    }

    // Save method
    [[nodiscard]] std::string to_txt_data() const override {
        std::stringstream ss;
        ss << "<LINE>\n" << title << '\n' <<
        std::to_string(x_label_number) << '\n' << std::to_string(i_pxl) << '\n';

        for (auto& [dataname, height] : label) {
            ss << dataname << '\n' << height << '\n';
        } ss << "</LINE>\n";

        std::string ret; ss >> ret;
        return ret;
    }

private:

    // This draws a line between the point (i1, j1) and (i2, j2)
    static void put_segment(Layer& frame, int i1, int j1, int i2, int j2) {

        for (int j = j1 + 1; j < j2; ++j) {
            int i = static_cast<int>(round((double)(i2 - i1) / (double)(j2 - j1) * (j - j1) + i1));
            frame[i][j] = '-';
        }
    }
    void put_line(Layer& frame) const override {

        // Set pivots
        vector<int> height_info = standardize();
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
            int i1 = i_pxl - 2 - height_info[p];
            int i2 = i_pxl - 2 - height_info[p + 1];
            int j1 = 2 + p * (max_len + 1) + max_len / 2;
            int j2 = 2 + (p + 1) * (max_len + 1) + max_len / 2;
            put_segment(frame, i1, j1, i2, j2);
        }
    }
};

class Document {
private:
    vector<Holder*> holders;
    string filename;
public:
    explicit Document(const vector<string>& data) : filename(data[0]) {
        /*
         vector<Holder*> 'holders' contains Holder* pointers, and each of it points an individual holder.
         To set a new holder, we push a new Holder* pointer and dynamically allocate it.
         */
        for (int i = 0; i < data.size(); ++i) {
            if (data[i] == "<TESTHOLDER>") {
                vector<string> hold;
                ++i;
                while (data[i] != "</TESTHOLDER>") {
                    hold.push_back(data[i++]);
                }
                Holder* p = new TestHolder(hold);
                holders.push_back(p);
                continue;
            }
            if (data[i] == "<SHAPE_HOLDER>") {
                vector<string> hold;
                ++i;
                while (data[i] != "</SHAPE_HOLDER>") {
                    hold.push_back(data[i++]);
                }
                Holder* p = new ShapeHolder(hold);
                holders.push_back(p);
                continue;
            }
            if (data[i] == "<STRINGHOLDER>") {
                vector<string> hold;
                ++i;
                while (data[i] != "</STRINGHOLDER>") {
                    hold.push_back(data[i++]);
                }
                Holder* p = new StringHolder(hold);
                holders.push_back(p);
                continue;
            }
            if (data[i] == "<LINE>") {
                vector<string> hold;
                ++i;
                while (data[i] != "</LINE>") {
                    hold.push_back(data[i++]);
                }
                Holder* p = new LineHolder(hold);
                holders.push_back(p);
                continue;
            }
            if (data[i] == "<HISTOGRAM>") {
                vector<string> hold;
                ++i;
                while (data[i] != "</HISTOGRAM>") {
                    hold.push_back(data[i++]);
                }
                Holder* p = new HistogramHolder(hold);
                holders.push_back(p);
                continue;
            }
            if (data[i] == "<TABLE>") {
                vector<string> hold;
                ++i;
                while (data[i] != "</TABLE>") {
                    hold.push_back(data[i++]);
                }
                Holder* p = new TableHolder(hold);
                holders.push_back(p);
                continue;
            }

        }
    }
    ~Document() {
        // Deallocate every pointers in 'holders' and pop it.
        for (int i = (int) holders.size() - 1; i >= 0; --i) {
            delete holders[i];
            holders.pop_back();
        }
    }

    // Print method
    void print() const {
        for (Holder* p : holders) {
            p->print();
        }
    }

    // Prints that the document is containing which holder.
    void print_infos() const {
        int i = 0;
        for (Holder* p : holders) {
            cout << "The holder #" << ++i;
            Holder::TYPE type = p->get_type();
            switch (type) {
                case Holder::TEST_HOLDER:
                    cout << " is a TestHolder"; break;
                case Holder::SHAPE_HOLDER:
                    cout << " is a ShapeHolder"; break;
                case Holder::STRING_HOLDER:
                    cout << " is a StringHolder"; break;
                case Holder::LINE_HOLDER:
                    cout << " is a LineHolder"; break;
                case Holder::HISTOGRAM_HOLDER:
                    cout << " is a HistogramHolder"; break;
                case Holder::TABLE_HOLDER:
                    cout << " is a TableHolder"; break;
                default:
                    cout << " RAISED AN ERROR : UNKNOWN HOLDER"; break;
            }
            cout << " named " << p->get_title() << ".\n";
        } cout << endl;
    }

    // Creates new empty holder at the end of the document.
    void push_tes() {
        Holder* p = new TestHolder;
        holders.push_back(p);
    }
    void push_str() {
        Holder* p = new StringHolder;
        holders.push_back(p);
    }
    void push_tab(int i = 1, int j = 1) {
        Holder* p = new TableHolder(i, j);
        holders.push_back(p);
    }
    void push_lin() {
        Holder* p = new LineHolder;
        holders.push_back(p);
    }
    void push_his() {
        Holder* p = new HistogramHolder;
        holders.push_back(p);
    }

    // Creates new empty holder between holders and holders.
    void push_tes_infrontof(int idx) {
        if (idx >= holders.size()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Document::push_tes_infrontof");
            return;
        }

        Holder* p = new TestHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_str_infrontof(int idx) {
        if (idx >= holders.size()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Document::push_str_infrontof");
            return;
        }

        Holder* p = new StringHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_tab_infrontof(int idx, int i = 1, int j = 1) {
        if (idx >= holders.size()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Document::push_tab_infrontof");
            return;
        }

        Holder* p = new TableHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_lin_infrontof(int idx) {
        if (idx >= holders.size()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Document::push_lin_infrontof");
            return;
        }

        Holder* p = new LineHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_his_infrontof(int idx) {
        if (idx >= holders.size()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Document::push_his_infrontof");
            return;
        }

        Holder* p = new HistogramHolder;
        holders.insert(holders.begin() + idx, p);
    }

    // Deletes a holder.
    void pop_holder() {
        if (holders.empty()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Document::pop_holder");
            return;
        }

        if (holders.back() != nullptr) {
            delete holders.back();
        }
        holders.pop_back();
    }

    void remove_holder(int idx) {
        if (idx >= holders.size()) {
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Document::remove_holder");
            return;
        }

        Holder*& to_be_del = holders[idx];
        if (to_be_del != nullptr) {
            delete to_be_del;
        }
        holders.erase(holders.begin() + idx);
    }

    // Save.
    [[nodiscard]] string save() const {
        string str;
        str += filename;
        str += "\n\n";
        for (Holder* p : holders) {
            str += p->to_txt_data();
        }
        return str;
    }

    // Access to individual holders.
    Holder* at(const int idx) {
        return holders.at(idx);
    }

    // get functions
    [[nodiscard]] unsigned int size() const {
        return (unsigned int) holders.size();
    }
    [[nodiscard]] string get_filename() const {
        return filename;
    }
};

class Listener {
    Clipboard clipboard;
    Document* doc_ptr;
    Trie* trie_ptr1;
    Trie* trie_ptr2;
    int how_many_words_do_you_want;

public:

    Listener(const vector<string>& sd, const vector<string>& strd) : how_many_words_do_you_want(10), doc_ptr(new Document(sd)), trie_ptr1(new Trie(strd)), trie_ptr2(new Trie(strd, [](const string& str) -> string {return mints::reversed(str);}, [](const string& str) -> string {return mints::reversed(str);})) {}

    ~Listener() {
        if (doc_ptr != nullptr) {
            delete doc_ptr;
        }
        if (trie_ptr1 != nullptr) {
            delete trie_ptr1;
        }
        if (trie_ptr2 != nullptr) {
            delete trie_ptr2;
        }
    }

    void listen() {
        int command, idx, new_holder_type;
        unsigned int _i, _j;

        cout << "Accessed to " << doc_ptr->get_filename() << " file.\n";
        cout << "To see the manual, input 0.\n\n";

        while (true) {
            cout << "at Document-modifier INPUT COMMAND : ";
            cin >> command;
            cin.ignore(1000, '\n');

            if (command < 0) {
                break;
            }

            try {
                switch (command) {
                    case 0:
                        print_manual(); break;

                    case 1:
                        doc_ptr->print(); break;

                    case 2:
                        doc_ptr->print_infos(); break;

                    case 10:
                        cout << "Put an index number to print : ";
                        cin >> idx;
                        cin.ignore(1000, '\n');
                        --idx;

                        if (idx >= doc_ptr->size()) {
                            throw ERRORS(OUT_OF_RANGE_ERROR, "at Listener::listen, case 10");
                        }

                        doc_ptr->at(idx)->print(); break;

                    case 11:
                        cout << "Put an index number to access : ";
                        cin >> idx;
                        cin.ignore(1000, '\n');
                        --idx;

                        if (idx >= doc_ptr->size()) {
                            throw ERRORS(OUT_OF_RANGE_ERROR, "at Listener::listen, case 11");
                        }

                        access(idx); break;

                    case 20:
                        cout << "Choose the type of new holder to push :" << endl;
                        cout << "1 StringHolder\t 2 TableHolder\t 3 LineHolder\t 4 HistogramHolder\t -1 cancel\t >> ";
                        cin >> new_holder_type;
                        cin.ignore(1000, '\n');

                        switch (new_holder_type) {
                            case -1234:
                                doc_ptr->push_tes(); break;

                            case 1:
                                doc_ptr->push_str(); break;

                            case 2:
                                cout << "Input the number of rows and columns in order : ";
                                cin >> _i >> _j;
                                cin.ignore(1000, '\n');

                                if (_i >= 70 || _j >= 15) {
                                    cout << "The number of rows and columns are restricted under 70 and 15 respectively.";
                                    throw ERRORS(TOO_LARGE_ARGUMENT_ERROR, "at Listener::listen, case 20-2");
                                }

                                doc_ptr->push_tab(_i, _j); break;

                            case 3:
                                doc_ptr->push_lin(); break;

                            case 4:
                                doc_ptr->push_his(); break;

                            default:
                                break;
                        }

                        break;

                    case 21:
                        cout << "Choose the type of new holder to push :" << endl;
                        cout << "1 StringHolder\t 2 TableHolder\t 3 LineHolder\t 4 HistogramHolder\t -1 cancel\t >> ";
                        cin >> new_holder_type;
                        cin.ignore(1000, '\n');
                        cout << "Enter the number immediately after the location you want to insert : ";
                        cin >> idx;
                        cin.ignore(1000, '\n');
                        --idx;

                        switch (new_holder_type) {
                            case -1234:
                                doc_ptr->push_tes_infrontof(idx); break;

                            case 1:
                                doc_ptr->push_str_infrontof(idx); break;

                            case 2:
                                cout << "Input the number of rows and columns in order : ";
                                cin >> _i >> _j;
                                cin.ignore(1000, '\n');

                                if (_i >= 70 || _j >= 15) {
                                    cout << "The number of rows and columns are restricted under 70 and 15 respectively.";
                                    throw ERRORS(TOO_LARGE_ARGUMENT_ERROR, "at Listener::listen, case 21-2");
                                }

                                doc_ptr->push_tab_infrontof(idx, _i, _j); break;

                            case 3:
                                doc_ptr->push_lin_infrontof(idx); break;

                            case 4:
                                doc_ptr->push_his_infrontof(idx); break;

                            default:
                                break;
                        }

                        break;

                    case 23:
                        int idx_to_delete;
                        cout << "Input the index of holder to delete : ";
                        cin >> idx_to_delete;
                        cin.ignore(1000, '\n');
                        --idx_to_delete;

                        if (idx_to_delete >= doc_ptr->size()) {
                            throw ERRORS(OUT_OF_RANGE_ERROR, "at Listener::listen, case 22");
                        }

                        doc_ptr->remove_holder(idx_to_delete); break;

                    default:
                        break;
                }
            } catch (ERRORS errors) {
                switch (errors.error_info) {
                    case OUT_OF_RANGE_ERROR:
                        cout << "OUT_OF_RANGE_ERROR OCCURED : " << errors.error_message << endl; break;

                    case UNPOPPABLE_CONTAINER_ERROR:
                        cout << "UNPOPPABLE_CONTAINER_ERROR OCCURED : " << errors.error_message << endl; break;

                    case TOO_LARGE_ARGUMENT_ERROR:
                        cout << "TOO_LARGE_ARGUMENT_ERROR OCCURED : " << errors.error_message << endl; break;

                    case NIL:
                        cout << errors.error_message << endl; break;
                }
            }
        }

        save();
    }

private:

    void print_manual() const {
        cout << "Put 1 to print all CONTENTS in the document." << endl;
        cout << "Put 2 to print all TYPES and TITLES of holders in the document." << endl;

        cout << endl;

        cout << "Put 10 to print an individual holder." << endl;
        cout << "Put 11 to access and modify an individual holder." << endl;

        cout << endl;

        cout << "Put 20 to make a new holder at the END of the document." << endl;
        cout << "Put 21 to add a new holder BETWEEN holders and holders." << endl;
        cout << "Put 22 to pop a holder at the END of the document." << endl;
        cout << "Put 23 to delete a holder in any place." << endl;

        cout << endl;

        cout << "Put -1 to exit the program." << endl << endl;
    }

    // Access functions
    void access(int n) {
        if (n >= doc_ptr->size()) {
            // Wrong access; Do nothing
            throw ERRORS(OUT_OF_RANGE_ERROR, "at Listener::access");
        }

        Holder* p = doc_ptr->at(n);

        // Check the type of holder and call prefer access functions.
        if (p->get_type() == Holder::STRING_HOLDER) {
            StringHolder* q = dynamic_cast<StringHolder*>(p);
            modify_str(q);
            return;
        }
        if (p->get_type() == Holder::TABLE_HOLDER) {
            TableHolder* q = dynamic_cast<TableHolder*>(p);
            modify_tab(q);
            return;
        }
        if (p->get_type() == Holder::LINE_HOLDER || p->get_type() == Holder::HISTOGRAM_HOLDER) {
            LineHolder* q = dynamic_cast<LineHolder*>(p);
            modify_cha(q);
            return;
        }

        throw ERRORS(NIL, "at Listener::access, NOT_IMPLEMENTED_ERROR: Sorry, the method to modify this holder is not implemented yet.");
        return;
    }

    void modify_str(StringHolder* p) {
        cout << "Accessed to the StringHolder named " << p->get_title() << endl;
        cout << "To see the manual, input 0.\n\n";

        int command, idx, len;
        string input_str;
        while (true) {

            p->print();

            cout << "at StringHolder-modifier INPUT COMMAND : ";
            cin >> command;
            cin.ignore(1000, '\n');

            if (command < 0) {
                break;
            }

            switch (command) {
                case 0:
                    print_str_manual(); break;

                case 1:
                    p->print(); break;

                case 3:
                    cout << "Put your new title : ";
                    getline(cin, input_str);
                    cin.ignore(1000, '\n');
                    p->set_title(input_str); break;

                case 10:
                    cout << "Put your text you want to push back : " << endl;
                    getline(cin, input_str);
                    p->push(input_str); break;

                case 11:
                    cout << "Put the index where you want to insert : ";
                    cin >> idx;
                    cin.ignore(1000, '\n');
                    --idx;
                    cout << "Put your text you want to push back : " << endl;
                    getline(cin, input_str);
                    p->insert(idx, input_str); break;

                case 20:
                    cout << "Put the index where you want to delete : ";
                    cin >> idx;
                    cin.ignore(1000, '\n');
                    --idx;

                    if (idx >= p->get_size()) {
                        cout << "Invalid index" << endl; break;
                    }

                    cout << "Put the length of text you want to delete : ";
                    cin >> len;
                    cin.ignore(1000, '\n');

                    p->remove(idx, len); break;

                case 21:
                    cout << "Put the index where you want to replace : ";
                    cin >> idx;
                    cin.ignore(1000, '\n');
                    --idx;

                    if (idx >= p->get_size()) {
                        cout << "Invalid index"; break;
                    }

                    cout << "Put the text you want to insert : ";
                    getline(cin, input_str);

                    p->insert(idx, input_str); break;

                case 50:
                    p->title_on(); break;

                case 51:
                    p->title_off(); break;

                case 99:
                    p->spellcheck(*trie_ptr1, *trie_ptr2, how_many_words_do_you_want); break;

                default:
                    break;
            }
        }
    }

    static void print_str_manual() {
        cout << "Put 1 to print all CONTENTS of the holder." << endl;
        cout << "Put 3 to change the title of holder." << endl;

        cout << endl;

        cout << "Put 10 to push your text at the back of the string." << endl;
        cout << "Put 11 to insert your text between the data." << endl;
        cout << "Put 20 to delete some elements between strings." << endl;
        cout << "Put 21 to replace some elements between string into other." << endl;
        cout << "Put 50 to print title." << endl;
        cout << "Put 51 to hide title." << endl;

        cout << endl;

        cout << "Put 99 to operate the spell-check function." << endl;
        cout << "Put -1 to break." << endl;
    }

    void modify_tab(TableHolder* p) {
        cout << "Accessed to the TableHolder named " << p->get_title() << endl;
        cout << "To see the manual, input 0.\n\n";

        int command, _i, _j;
        string input_str;

        while (true) {

            p->print();

            cout << "at TableHolder-modifier INPUT COMMAND : ";
            cin >> command;
            cin.ignore(1000, '\n');

            if (command < 0) {
                break;
            }

            switch (command) {
                case 0:
                    print_tab_manual(); break;

                case 1:
                    p->print(); break;

                case 3:
                    cout << "Put your new title : ";
                    getline(cin, input_str);

                    p->set_title(input_str); break;

                case 10:
                    cout << "Put the index number of rows and columns in order : ";
                    cin >> _i >> _j;
                    cin.ignore(1000, '\n');
                    --_i; --_j;

                    cout << "Put the text you want to replace : " << endl;
                    getline(cin, input_str);

                    p->put(input_str, _i, _j); break;

                case 50:
                    p->title_on(); break;

                case 51:
                    p->title_off(); break;

                default:
                    break;
            }
        }
    }

    static void print_tab_manual() {
        cout << "Put 1 to print all CONTENTS of the holder." << endl;
        cout << "Put 3 to change the title of holder." << endl;

        cout << endl;

        cout << "Put 10 to change an element in the table." << endl;
        cout << "Put 50 to print title." << endl;
        cout << "Put 51 to hide title." << endl;

        cout << endl;

        cout << "Put -1 to break." << endl;
    }

    void modify_cha(ChartHolder* p) {
        cout << "Accessed to the ChartHolder named " << p->get_title() << endl;
        cout << "To see the manual, input 0.\n\n";

        int command, height, idx;
        double data;
        string input_str;

        while (true) {

            p->print();

            cout << "at ChartHolder-modifier INPUT COMMAND : ";
            cin >> command;
            cin.ignore(1000, '\n');

            if (command < 0) {
                break;
            }

            switch (command) {
                case 0:
                    print_cha_manual();
                    break;

                case 1:
                    p->print();
                    break;

                case 3:
                    cout << "Put your new title : ";
                    getline(cin, input_str);

                    p->set_title(input_str);
                    break;

                case 10:
                    cout << "Put the text you want to push : ";
                    getline(cin, input_str);

                    cout << "Put the number you want to push : ";
                    cin >> data;
                    cin.ignore(1000, '\n');

                    p->push(input_str, data);
                    break;

                case 11:
                    p->pop();
                    break;

                case 20:
                    cout << "Put the vertical size you want : ";
                    cin >> height;
                    cin.ignore(1000, '\n');

                    p->modify_i_size(height);
                    break;

                case 21:
                    cout << "Put the index number : ";
                    cin >> idx;
                    cin.ignore(1000, '\n');
                    --idx;

                    cout << "Put the text you want to replace : ";
                    getline(cin, input_str);

                    p->modify_dataname(input_str, idx);
                    break;

                case 22:
                    cout << "Put the index number : ";
                    cin >> idx;
                    cin.ignore(1000, '\n');
                    --idx;

                    cout << "Put the height you want to replace : ";
                    cin >> height;
                    cin.ignore(1000, '\n');

                    p->modify_height(height, idx);
                    break;

                case 50:
                    p->title_on();
                    break;

                case 51:
                    p->title_off();
                    break;

                default:
                    break;
            }
        }
    }

    static void print_cha_manual() {
        cout << "Put 1 to print all CONTENTS of the holder." << endl;
        cout << "Put 3 to change the title of holder." << endl;

        cout << endl;

        cout << "Put 10 to push a data." << endl;
        cout << "Put 11 to pop a data." << endl;
        cout << "Put 20 to modify the vertical size of the graph." << endl;
        cout << "Put 21 to modify the data name." << endl;
        cout << "Put 22 to modify the height." << endl;
        cout << "Put 50 to print title." << endl;
        cout << "Put 51 to hide title." << endl;

        cout << endl;

        cout << "Put -1 to break." << endl;
    }

    void save() {
        ofstream ofile("tester.txt");
        ofile << doc_ptr->save();

        cout << "The document is successfully saved." << endl;
    }
};

int main() {
    ifstream ifile("../tester.txt");
    string str; vector<string> scanned_data;

    if (!ifile) {
        cout << "Unable to open document file." << endl;
        return 0;
    }

    while (getline(ifile, str)) {
        scanned_data.push_back(str);
    }

    ifstream triefile("../dict.txt");
    vector<string> scanned_trie_data;

    if (!triefile) {
        cout << "Unable to open word list file." << endl;
        return 0;
    }

    while (getline(triefile, str)) {
        string lower_str = mints::make_lowercase(str);
        scanned_trie_data.push_back(lower_str);
    }

    Listener listener(scanned_data, scanned_trie_data);
    listener.listen();

    cout << "The program is successfully closed." << endl;
}

