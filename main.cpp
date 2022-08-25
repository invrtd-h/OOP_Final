/**
 * OOPFinal Mint Choco 프로그램은 보고서 작성에 사용되는 다양한 기능을 제공합니다.
 *
 * @version 22.08.25
 * @author 송혜근
 * @author 윤세림
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Node;
class Trie;
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

int main() {
    std::ifstream ifile("../tester.txt"), triefile("../dict.txt");
    std::string str; std::vector<std::string> scanned_data, scanned_trie_data;

    if (not ifile) {
        throw mints::unable_to_open_file("Unable to open file : {name : tester.txt}");
    }
    if (not triefile) {
        throw mints::unable_to_open_file("Unable to open file : {name : dict.txt}");
    }

    while (getline(ifile, str)) {
        scanned_data.push_back(str);
    }
    while (getline(triefile, str)) {
        std::string lower_str = mints::make_lowercase(str);
        scanned_trie_data.push_back(lower_str);
    }

    Listener listener(scanned_data, scanned_trie_data);
    auto save_data = listener.listen();

    std::ofstream ofile("tester.txt");
    ofile << save_data;

    std::cout << "The program is successfully closed." << std::endl;
}

