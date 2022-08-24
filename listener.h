#ifndef OOPFINAL_LISTENER_H
#define OOPFINAL_LISTENER_H

class Listener {
    Document* doc_ptr;
    Trie* trie_ptr1;
    Trie* trie_ptr2;
    int how_many_words_do_you_want;

public:

    Listener(const std::vector<std::string>& sd, const std::vector<std::string>& strd)
    : how_many_words_do_you_want(10),
        doc_ptr(new Document(sd)),
        trie_ptr1(new Trie(strd)),
        trie_ptr2(new Trie(strd, [](const std::string& str) -> std::string {return mints::reversed(str);}, [](const std::string& str) -> std::string {return mints::reversed(str);})) {}

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

        std::cout << "Accessed to " << doc_ptr->get_filename() << " file.\n";
        std::cout << "To see the manual, input 0.\n\n";

        while (true) {
            std::cout << "at Document-modifier INPUT COMMAND : ";
            std::cin >> command;
            std::cin.ignore(1000, '\n');

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
                        std::cout << "Put an index number to print : ";
                        std::cin >> idx;
                        std::cin.ignore(1000, '\n');
                        --idx;

                        if (idx >= doc_ptr->size()) {
                            throw mints::input_out_of_range("at Listener::listen, case 10");
                        }

                        doc_ptr->at(idx)->print(); break;

                    case 11:
                        std::cout << "Put an index number to access : ";
                        std::cin >> idx;
                        std::cin.ignore(1000, '\n');
                        --idx;

                        if (idx >= doc_ptr->size()) {
                            throw mints::input_out_of_range("at Listener::listen, case 10");
                        }

                        access(idx); break;

                    case 20:
                        std::cout << "Choose the type of new holder to push :" << std::endl;
                        std::cout << "1 StringHolder\t 2 TableHolder\t 3 LineHolder\t 4 HistogramHolder\t -1 cancel\t >> ";
                        std::cin >> new_holder_type;
                        std::cin.ignore(1000, '\n');

                        switch (new_holder_type) {
                            case -1234:
                                doc_ptr->push_tes(); break;

                            case 1:
                                doc_ptr->push_str(); break;

                            case 2:
                                std::cout << "Input the number of rows and columns in order : ";
                                std::cin >> _i >> _j;
                                std::cin.ignore(1000, '\n');

                                if (_i >= 70 || _j >= 15) {
                                    std::cout << "The number of rows and columns are restricted under 70 and 15 respectively.";
                                    throw mints::input_out_of_range("at Listener::listen, case 20-2");
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
                        std::cout << "Choose the type of new holder to push :" << std::endl;
                        std::cout << "1 StringHolder\t 2 TableHolder\t 3 LineHolder\t 4 HistogramHolder\t -1 cancel\t >> ";
                        std::cin >> new_holder_type;
                        std::cin.ignore(1000, '\n');
                        std::cout << "Enter the number immediately after the location you want to insert : ";
                        std::cin >> idx;
                        std::cin.ignore(1000, '\n');
                        --idx;

                        switch (new_holder_type) {
                            case -1234:
                                doc_ptr->push_tes_infrontof(idx); break;

                            case 1:
                                doc_ptr->push_str_infrontof(idx); break;

                            case 2:
                                std::cout << "Input the number of rows and columns in order : ";
                                std::cin >> _i >> _j;
                                std::cin.ignore(1000, '\n');

                                if (_i >= 70 || _j >= 15) {
                                    std::cout << "The number of rows and columns are restricted under 70 and 15 respectively.";
                                    throw mints::input_out_of_range("at Listener::listen, case 21-2");
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
                        std::cout << "Input the index of holder to delete : ";
                        std::cin >> idx_to_delete;
                        std::cin.ignore(1000, '\n');
                        --idx_to_delete;

                        if (idx_to_delete >= doc_ptr->size()) {
                            throw mints::input_out_of_range("at Listener::listen, case 22");
                        }

                        doc_ptr->remove_holder(idx_to_delete); break;

                    default:
                        break;
                }
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
            }
        }

        save();
    }

private:

    static void print_manual() {
        std::cout << mint_manuals::general_manual;
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

        throw std::exception();
    }

    void modify_str(StringHolder* p) {
        std::cout << "Accessed to the StringHolder named " << p->get_title() << std::endl;
        std::cout << "To see the manual, input 0.\n\n";

        int command, idx, len;
        std::string input_str;
        while (true) {

            p->print();

            std::cout << "at StringHolder-modifier INPUT COMMAND : ";
            std::cin >> command;
            std::cin.ignore(1000, '\n');

            if (command < 0) {
                break;
            }

            switch (command) {
                case 0:
                    print_str_manual(); break;

                case 1:
                    p->print(); break;

                case 3:
                    std::cout << "Put your new title : ";
                    getline(std::cin, input_str);
                    std::cin.ignore(1000, '\n');
                    p->set_title(input_str); break;

                case 10:
                    std::cout << "Put your text you want to push back : " << std::endl;
                    getline(std::cin, input_str);
                    p->push(input_str); break;

                case 11:
                    std::cout << "Put the index where you want to insert : ";
                    std::cin >> idx;
                    std::cin.ignore(1000, '\n');
                    --idx;
                    std::cout << "Put your text you want to push back : " << std::endl;
                    getline(std::cin, input_str);
                    p->insert(idx, input_str); break;

                case 20:
                    std::cout << "Put the index where you want to delete : ";
                    std::cin >> idx;
                    std::cin.ignore(1000, '\n');
                    --idx;

                    if (idx >= p->get_size()) {
                        std::cout << "Invalid index" << std::endl; break;
                    }

                    std::cout << "Put the length of text you want to delete : ";
                    std::cin >> len;
                    std::cin.ignore(1000, '\n');

                    p->remove(idx, len); break;

                case 21:
                    std::cout << "Put the index where you want to replace : ";
                    std::cin >> idx;
                    std::cin.ignore(1000, '\n');
                    --idx;

                    if (idx >= p->get_size()) {
                        std::cout << "Invalid index"; break;
                    }

                    std::cout << "Put the text you want to insert : ";
                    getline(std::cin, input_str);

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
        std::cout << "Accessed to the TableHolder named " << p->get_title() << std::endl;
        std::cout << "To see the manual, input 0.\n\n";

        int command, _i, _j;
        std::string input_str;

        while (true) {

            p->print();

            std::cout << "at TableHolder-modifier INPUT COMMAND : ";
            std::cin >> command;
            std::cin.ignore(1000, '\n');

            if (command < 0) {
                break;
            }

            switch (command) {
                case 0:
                    print_tab_manual(); break;

                case 1:
                    p->print(); break;

                case 3:
                    std::cout << "Put your new title : ";
                    getline(std::cin, input_str);

                    p->set_title(input_str); break;

                case 10:
                    std::cout << "Put the index number of rows and columns in order : ";
                    std::cin >> _i >> _j;
                    std::cin.ignore(1000, '\n');
                    --_i; --_j;

                    std::cout << "Put the text you want to replace : " << std::endl;
                    getline(std::cin, input_str);

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
        std::cout << "Accessed to the ChartHolder named " << p->get_title() << std::endl;
        std::cout << "To see the manual, input 0.\n\n";

        int command, height, idx;
        double data;
        std::string input_str;

        while (true) {

            p->print();

            std::cout << "at ChartHolder-modifier INPUT COMMAND : ";
            std::cin >> command;
            std::cin.ignore(1000, '\n');

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
                    std::cout << "Put your new title : ";
                    getline(std::cin, input_str);

                    p->set_title(input_str);
                    break;

                case 10:
                    std::cout << "Put the text you want to push : ";
                    getline(std::cin, input_str);

                    std::cout << "Put the number you want to push : ";
                    std::cin >> data;
                    std::cin.ignore(1000, '\n');

                    p->push(input_str, data);
                    break;

                case 11:
                    p->pop();
                    break;

                case 20:
                    std::cout << "Put the vertical size you want : ";
                    std::cin >> height;
                    std::cin.ignore(1000, '\n');

                    p->modify_i_size(height);
                    break;

                case 21:
                    std::cout << "Put the index number : ";
                    std::cin >> idx;
                    std::cin.ignore(1000, '\n');
                    --idx;

                    std::cout << "Put the text you want to replace : ";
                    getline(std::cin, input_str);

                    p->modify_dataname(input_str, idx);
                    break;

                case 22:
                    std::cout << "Put the index number : ";
                    std::cin >> idx;
                    std::cin.ignore(1000, '\n');
                    --idx;

                    std::cout << "Put the height you want to replace : ";
                    std::cin >> height;
                    std::cin.ignore(1000, '\n');

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
        std::ofstream ofile("tester.txt");
        ofile << doc_ptr->save();

        std::cout << "The document is successfully saved." << std::endl;
    }
};

#endif //OOPFINAL_LISTENER_H
