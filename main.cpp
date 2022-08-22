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
class HistogramHolder;
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

    static void print_manual() {
        std::cout << mint_manuals::print_manual;
    }
    static void print_str_manual() {
        std::cout << mint_manuals::str_manual;
    }
    static void print_tab_manual() {
        std::cout << mint_manuals::tab_manual;
    }
    static void print_cha_manual() {
        std::cout << mint_manuals::cha_manual;
    }

    // Access functions
    void access(int n) {
        if (n >= doc_ptr->size()) {
            // Wrong access; Do nothing
            throw mints::input_out_of_range("at Listener::access");
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

