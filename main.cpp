#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <vector>

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
#include "docus.h"
#include "listener.h"

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

