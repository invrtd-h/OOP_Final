#include "trie.h"

Node::Node(char _c, int _level) : ch(_c), level(_level), to_data(nullptr), offspring_num(0) {
    for (int i = 0; i < 26; ++i) {
        next[i] = nullptr;
    }
}

Node::~Node() {
    // cout << ch << " Node with offspring " << offspring_num << " deleted" << endl;
    for (auto i : next) {
        if (i != nullptr) {
            delete i;
        }
    }
    if (to_data != nullptr) {
        delete to_data;
    }
}

void Node::put(char c) {
    if (next[c - 97] == nullptr) { // For some make_lowercase char c, (c - 97) converts a to 0, b to 1, ... z to 25
        next[c - 97] = new Node(c, level + 1);
        ++offspring_num;
    } else {
        throw mints::double_alloc("tried double alloc at Node::put, some logical error expected");
    }
}

void Node::put(const std::string &str) {
    to_data = new std::string(str);
    ++offspring_num;
}

void Node::remove(char c) {
    if (next[c - 97] != nullptr) {
        delete next[c - 97];
        next[c - 97] = nullptr;
        --offspring_num;
    } else {
        throw mints::double_free("tried double free at Node::remove, some logical error expected");
    }
}

void Node::remove() {
    if (to_data != nullptr) {
        delete to_data;
        to_data = nullptr;
        --offspring_num;
    } else {
        throw mints::double_free("tried double free at Node::remove, some logical error expected");
    }
}

char Node::get_char() const {
    return ch;
}

Node *Node::get_next(int idx) const {
    idx -= 97;
    if (0 <= idx && idx < 26) {
        return next[idx];
    }
    return nullptr;
}

int Node::get_level() const {
    return level;
}

int Node::get_offspring_num() const {
    return offspring_num;
}

std::string *Node::get_strptr() const {
    return to_data;
}

std::vector<std::string> Node::traverse(const int MAX_VEC_SIZE) const {
    std::vector<std::string> vecstr;
    vecstr.reserve(MAX_VEC_SIZE);

    // Do Depth-First-Search and get all strings contained at the end node.
    std::vector<const Node*> stack;
    stack.push_back(this);

    while (!stack.empty()) {
        const Node* popped = stack.back();
        stack.pop_back();

        if (popped->to_data != nullptr) {
            vecstr.push_back(*popped->to_data);
            if (vecstr.size() >= MAX_VEC_SIZE) {
                break;
            }
        }

        for (int i = 25; i >= 0; --i) {
            if (popped->next[i] != nullptr) {
                stack.push_back(popped->next[i]);
            }
        }
    }

    return vecstr;
}

void Node::depth_first_search(std::string &str) const {
    str += ch;
    for (auto i : next) {
        if (i != nullptr) {
            i->depth_first_search(str);
        }
    }
    if (97 <= str.back() && str.back() <= 122) {
        str += '0';
    } else {
        ++str.back();
    }
}

const Node *Trie::deepest_node_so_far(const std::string &str) const {
    const Node* travel = this;
    for (char c : str) {
        if (travel->get_next(c) == nullptr) {
            return travel;
        }
        travel = travel->get_next(c);
    }
    return travel;
}

const Node *Trie::if_contained_get_lowest_nonbranch(const std::string &str) const {
    // A stack containing all travelled node so far
    std::vector<const Node*> nodes_so_far;

    const Node* travel = this;
    nodes_so_far.push_back(travel);
    for (char c : str) {
        if (travel->get_next(c) == nullptr) {
            // If str is not contained in our Trie, then return NULL
            return nullptr;
        }
        travel = travel->get_next(c);
        nodes_so_far.push_back(travel);
    }

    if (*travel->get_strptr() == str) {
        while (nodes_so_far.back()->get_offspring_num() == 1) {
            nodes_so_far.pop_back();
        }
        if (nodes_so_far.empty()) {
            return this;
        } else {
            return nodes_so_far.back();
        }
    }

    return nullptr;
}

bool Trie::_contains_(const std::string &input) const {
    std::string str = preprocess(input);
    // Check whether the pushed string does not contain non-make_lowercase-alphabet characters
    for (char c : str) {
        if (c < 97 || c > 122) { // 97 == 'a', 122 == 'z'
            // if a non-make_lowercase-alphabet character is found, then it is not contained in our Trie.
            return false;
        }
    }

    // Delete the const feature; now ptr is no more const Node* pointer.
    Node* ptr = const_cast<Node*>(deepest_node_so_far(str));

    if (ptr->get_level() == str.size()) {
        std::string* temp = ptr->get_strptr();
        if (temp != nullptr && *temp == str) {
            // If we found the str in our Trie, then return true.
            return true;
        }
    }

    // Otherwise, return false.
    return false;
}

void Trie::push(const std::string &input) {
    const std::string str = preprocess(input);
    // Check whether the pushed string does not contain non-make_lowercase-alphabet characters
    for (char c : str) {
        if (c < 97 || c > 122) { // 97 == 'a', 122 == 'z'
            // if a non-make_lowercase-alphabet character is found, then do nothing
            return;
        }
    }

    // Delete the const feature; now ptr is no more const Node* pointer.
    Node* ptr = const_cast<Node*>(deepest_node_so_far(str));

    if (ptr->get_level() == str.size()) {
        std::string* temp = ptr->get_strptr();
        if (temp != nullptr && *temp == str) {
            // If str already exists in our Trie, then do nothing.
            return;
        }
    }

    // Input new nodes along the given string.
    for (int i = ptr->get_level(); i < str.size(); ++i) {
        ptr->put(str[i]);
        ptr = ptr->get_next(str[i]);
    }

    // Input a string at end node, to mark that this is the end node.
    ptr->put(str);
}

void Trie::remove(const std::string &str) {
    // Check whether the pushed string does not contain non-make_lowercase-alphabet characters
    for (char c : str) {
        if (c < 97 || c > 122) { // 97 == 'a', 122 == 'z'
            // if a non-make_lowercase-alphabet character is found, then our Trie may do not contain it: Do nothing.
            return;
        }
    }

    // Delete the const feature; now ptr is no more const Node* pointer.
    Node* ptr = const_cast<Node*>(if_contained_get_lowest_nonbranch(str));

    if (ptr == nullptr) {
        // If str is not contained in our Trie, then do nothing.
        return;
    }

    // If str is contained in our Trie, and there is no branch on our path, then just delete
    if (ptr->get_level() == str.size()) {
        ptr->remove();
    } else {
        // If str is contained in our Trie, and there is a branch on our path, then delete the branch
        ptr->remove(str[ptr->get_level()]);
    }
}

std::vector<std::string> Trie::get_suggestions(const std::string &input, const int MAX_SUGGESTIONS) const {
    const std::string to_search = preprocess(input);

    // Find the closest prefix in our dictionary
    const Node* search_start_node = deepest_node_so_far(to_search);
    // Load all strings with same prefixes in our dictionary
    std::vector<std::string> ret = search_start_node->traverse(MAX_SUGGESTIONS);

    for (auto& i : ret) {
        i = backprocess(i);
    }

    return ret;
}

std::string Trie::to_txt_data() const {
    std::string txt_holder;

    depth_first_search(txt_holder);

    return txt_holder;
}

auto Trie::get_preprocess() const {
    return preprocess;
}

auto Trie::get_backprocess() const {
    return backprocess;
}
