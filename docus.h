#ifndef OOPFINAL_DOCUS_H
#define OOPFINAL_DOCUS_H

class Document {
private:
    std::vector<Holder*> holders;
    std::string filename;
public:
    explicit Document(const std::vector<std::string>& data) : filename(data[0]) {
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
            throw mints::input_out_of_range("at Document::push_tes_infrontof");
        }

        Holder* p = new TestHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_str_infrontof(int idx) {
        if (idx >= holders.size()) {
            throw mints::input_out_of_range("at Document::push_str_infrontof");
        }

        Holder* p = new StringHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_tab_infrontof(int idx, int i = 1, int j = 1) {
        if (idx >= holders.size()) {
            throw mints::input_out_of_range("at Document::push_tab_infrontof");
        }

        Holder* p = new TableHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_lin_infrontof(int idx) {
        if (idx >= holders.size()) {
            throw mints::input_out_of_range("at Document::push_lin_infrontof");
        }

        Holder* p = new LineHolder;
        holders.insert(holders.begin() + idx, p);
    }
    void push_his_infrontof(int idx) {
        if (idx >= holders.size()) {
            throw mints::input_out_of_range("at Document::push_his_infrontof");
        }

        Holder* p = new HistogramHolder;
        holders.insert(holders.begin() + idx, p);
    }

    // Deletes a holder.
    void pop_holder() {
        if (holders.empty()) {
            throw mints::input_out_of_range("at Document::pop_holder");
        }

        if (holders.back() != nullptr) {
            delete holders.back();
        }
        holders.pop_back();
    }

    void remove_holder(int idx) {
        if (idx >= holders.size()) {
            throw mints::input_out_of_range("at Document::remove_holder");
        }

        Holder*& to_be_del = holders[idx];
        if (to_be_del != nullptr) {
            delete to_be_del;
        }
        holders.erase(holders.begin() + idx);
    }

    // Save.
    [[nodiscard]] std::string save() const {
        std::stringstream ss;
        ss << filename << "\n\n";
        for (Holder* p : holders) {
            ss << p->to_txt_data();
        }

        std::string str; ss >> str;
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
    [[nodiscard]] std::string get_filename() const {
        return filename;
    }
};

#endif //OOPFINAL_DOCUS_H
