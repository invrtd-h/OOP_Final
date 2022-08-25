#include "docus.h"

Document::Document(const std::vector<std::string> &data) : filename(data[0]) {
    /*
     vector<Holder*> 'holders' contains Holder* pointers, and each of it points an individual holder.
     To set a new holder, we push a new Holder* pointer and dynamically allocate it.
     */
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == "<TESTHOLDER>") {
            std::vector<std::string> hold;
            ++i;
            while (data[i] != "</TESTHOLDER>") {
                hold.push_back(data[i++]);
            }
            Holder* p = new TestHolder(hold);
            holders.push_back(p);
            continue;
        }
        if (data[i] == "<SHAPE_HOLDER>") {
            std::vector<std::string> hold;
            ++i;
            while (data[i] != "</SHAPE_HOLDER>") {
                hold.push_back(data[i++]);
            }
            Holder* p = new ShapeHolder(hold);
            holders.push_back(p);
            continue;
        }
        if (data[i] == "<STRINGHOLDER>") {
            std::vector<std::string> hold;
            ++i;
            while (data[i] != "</STRINGHOLDER>") {
                hold.push_back(data[i++]);
            }
            Holder* p = new StringHolder(hold);
            holders.push_back(p);
            continue;
        }
        if (data[i] == "<LINE>") {
            std::vector<std::string> hold;
            ++i;
            while (data[i] != "</LINE>") {
                hold.push_back(data[i++]);
            }
            Holder* p = new LineHolder(hold);
            holders.push_back(p);
            continue;
        }
        if (data[i] == "<HISTOGRAM>") {
            std::vector<std::string> hold;
            ++i;
            while (data[i] != "</HISTOGRAM>") {
                hold.push_back(data[i++]);
            }
            Holder* p = new HistogramHolder(hold);
            holders.push_back(p);
            continue;
        }
        if (data[i] == "<TABLE>") {
            std::vector<std::string> hold;
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

Document::~Document() {
    // Deallocate every pointers in 'holders' and pop it.
    for (int i = (int) holders.size() - 1; i >= 0; --i) {
        delete holders[i];
        holders.pop_back();
    }
}

void Document::print() const {
    for (Holder* p : holders) {
        p->print();
    }
}

void Document::print_infos() const {
    int i = 0;
    for (Holder* p : holders) {
        std::cout << "The holder #" << ++i;
        Holder::TYPE type = p->get_type();
        switch (type) {
            case Holder::TEST_HOLDER:
                std::cout << " is a TestHolder"; break;
            case Holder::SHAPE_HOLDER:
                std::cout << " is a ShapeHolder"; break;
            case Holder::STRING_HOLDER:
                std::cout << " is a StringHolder"; break;
            case Holder::LINE_HOLDER:
                std::cout << " is a LineHolder"; break;
            case Holder::HISTOGRAM_HOLDER:
                std::cout << " is a HistogramHolder"; break;
            case Holder::TABLE_HOLDER:
                std::cout << " is a TableHolder"; break;
            default:
                std::cout << " RAISED AN ERROR : UNKNOWN HOLDER"; break;
        }
        std::cout << " named " << p->get_title() << ".\n";
    } std::cout << std::endl;
}

void Document::push_tes() {
    Holder* p = new TestHolder;
    holders.push_back(p);
}

void Document::push_str() {
    Holder* p = new StringHolder;
    holders.push_back(p);
}

void Document::push_tab(int i, int j) {
    Holder* p = new TableHolder(i, j);
    holders.push_back(p);
}

void Document::push_lin() {
    Holder* p = new LineHolder;
    holders.push_back(p);
}

void Document::push_his() {
    Holder* p = new HistogramHolder;
    holders.push_back(p);
}

void Document::push_tes_infrontof(int idx) {
    if (idx >= holders.size()) {
        throw mints::input_out_of_range("at Document::push_tes_infrontof");
    }

    Holder* p = new TestHolder;
    holders.insert(holders.begin() + idx, p);
}

void Document::push_str_infrontof(int idx) {
    if (idx >= holders.size()) {
        throw mints::input_out_of_range("at Document::push_str_infrontof");
    }

    Holder* p = new StringHolder;
    holders.insert(holders.begin() + idx, p);
}

void Document::push_tab_infrontof(int idx, int i, int j) {
    if (idx >= holders.size()) {
        throw mints::input_out_of_range("at Document::push_tab_infrontof");
    }

    Holder* p = new TableHolder;
    holders.insert(holders.begin() + idx, p);
}

void Document::push_lin_infrontof(int idx) {
    if (idx >= holders.size()) {
        throw mints::input_out_of_range("at Document::push_lin_infrontof");
    }

    Holder* p = new LineHolder;
    holders.insert(holders.begin() + idx, p);
}

void Document::push_his_infrontof(int idx) {
    if (idx >= holders.size()) {
        throw mints::input_out_of_range("at Document::push_his_infrontof");
    }

    Holder* p = new HistogramHolder;
    holders.insert(holders.begin() + idx, p);
}

void Document::pop_holder() {
    if (holders.empty()) {
        throw mints::input_out_of_range("at Document::pop_holder");
    }

    if (holders.back() != nullptr) {
        delete holders.back();
    }
    holders.pop_back();
}

void Document::remove_holder(int idx) {
    if (idx >= holders.size()) {
        throw mints::input_out_of_range("at Document::remove_holder");
    }

    Holder*& to_be_del = holders[idx];
    if (to_be_del != nullptr) {
        delete to_be_del;
    }
    holders.erase(holders.begin() + idx);
}

std::string Document::save() const {
    std::stringstream ss;
    ss << filename << "\n\n";
    for (Holder* p : holders) {
        ss << p->to_txt_data();
    }

    std::string str; ss >> str;
    return str;
}

Holder *Document::at(int idx) {
    return holders.at(idx);
}

unsigned int Document::size() const {
    return (unsigned int) holders.size();
}

std::string Document::get_filename() const {
    return filename;
}
