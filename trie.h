#ifndef OOPFINAL_TRIE_H
#define OOPFINAL_TRIE_H

#include <functional>
#include <memory>
#include <string>
#include <array>
#include <utility>

#include "mint_utils.h"

class Node {
protected:
    char ch;
    int level, offspring_num;
    Node* next[26];
    std::string* to_data;

public:
    Node(char _c, int _level) : ch(_c), level(_level), to_data(nullptr), offspring_num(0) {
        for (auto& i : next) {
            i = nullptr;
        }
    }
    virtual ~Node() {
        // cout << ch << " Node with offspring " << offspring_num << " deleted" << endl;
        for (auto& i : next) {
            if (i != nullptr) {
                delete i;
            }
        }
        if (to_data != nullptr) {
            delete to_data;
        }
    }

    Node(const Node&) = delete;
    Node(const Node&&) = delete;
    Node& operator=(const Node&) = delete;

    // Add some nodes
    void put(char c) {
        if (next[c - 97] == nullptr) { // For some make_lowercase char c, (c - 97) converts a to 0, b to 1, ... z to 25
            next[c - 97] = new Node(c, level + 1);
            ++offspring_num;
        } else {
            throw mints::double_alloc("tried double alloc at Node::put, some logical error expected");
        }
    }
    // Add a string at end node : The node with non-NULL string* to_data means that it is the end node.
    void put(const std::string& str) {
        to_data = new std::string(str);
        ++offspring_num;
    }
    // Delete an offspring node
    void remove(char c) {
        if (next[c - 97] != nullptr) {
            delete next[c - 97];
            next[c - 97] = nullptr;
            --offspring_num;
        } else {
            throw mints::double_free("tried double free at Node::remove, some logical error expected");
        }
    }
    // Delete string*
    void remove() {
        if (to_data != nullptr) {
            delete to_data;
            to_data = nullptr;
            --offspring_num;
        } else {
            throw mints::double_free("tried double free at Node::remove, some logical error expected");
        }
    }
    // Get functions
    [[nodiscard]] char get_char() const {
        return ch;
    }
    [[nodiscard]] Node* get_next(int idx) const {
        idx -= 97;
        if (0 <= idx && idx < 26) {
            return next[idx];
        }
        return nullptr;
    }
    [[nodiscard]] int get_level() const {
        return level;
    }
    [[nodiscard]] int get_offspring_num() const {
        return offspring_num;
    }
    [[nodiscard]] std::string* get_strptr() const {
        return to_data;
    }
    // Get functions end

    // Traversal function
    [[nodiscard]] std::vector<std::string> traverse(const int MAX_VEC_SIZE) const {
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

    // Depth-First-Search method.
protected:
    void depth_first_search(std::string& str) const {
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
};

class Trie : Node {
    /*
     "Trie" class is used to implement a powerful data structure called "Trie" : Check the description of "Node" class.
     "Trie" class inherits "Node" class, and it has no additional data but has some useful methods:
     We did this due to obey the Data-hiding rule. We want to apply these methods only at the root node of "Trie".
     If not, we will get a garbage value.
     */
    std::function<std::string(std::string)> preprocess, backprocess;
public:
    explicit Trie(std::function<std::string(std::string)> f,
         std::function<std::string(std::string)> g)
            : Node(35, 0), preprocess(std::move(f)), backprocess(std::move(g)) {}
            // Character number 35 means "#"; Initializes the top node
    explicit Trie(const std::vector<std::string>& vs,
         std::function<std::string(std::string)> f,
         std::function<std::string(std::string)> g)
            : Node(35, 0), preprocess(std::move(f)), backprocess(std::move(g)) {
        for (const auto& str : vs) {
            push(str);
        }
    }

    Trie(const Trie&) = delete;
    Trie(const Trie&&) = delete;
    Trie& operator=(const Trie&) = delete;

private:
    /*
     deepest_node_so_far searches the 'nearest' node with the given string.
     Here, we define the 'closeness' of two string as the length of the longest common prefix: i.e. the 'closeness' between 'standard' and 'stainless' is 3 == len(sta).
     i.e. if the Trie is given like this:
            #      (Lv.0)
           / \
          s   r    (Lv.1)
         /   / \
        t   g   h  (Lv.2)
       =st =rg =rh
     then deepest_node_so_far(rg) returns g at Lv.2;
     deepest_node_so_far(rt) returns r at Lv.1;
     deepest_node_so_far(tr) returns # at Lv.0.
     */
    [[nodiscard]] const Node* deepest_node_so_far(const std::string& str) const {
        const Node* travel = this;
        for (char c : str) {
            if (travel->get_next(c) == nullptr) {
                return travel;
            }
            travel = travel->get_next(c);
        }
        return travel;
    }

    /*
     if_contained_get_lowest_nonbranch(str) returns the lowest nonbranch node on the way to 'str' node if 'str' is in our Tree, otherwise return NULL.
     Here, the definition of "branch node" is the node whose all subnodes have only 1 connected offspring.
     i.e. if the Trie is given like this:
            #      (Lv.0)
           / \
          s   r    (Lv.1)
         /   / \
        t   g   h  (Lv.2)
       =st =rg =rh
     then if_contained_get_lowest_nonbranch(rh) returns r at Lv.1;
     if_contained_get_lowest_nonbranch(st) returns # at Lv.0;
     if_contained_get_lowest_nonbranch(sg) returns NULL.
     */
    [[nodiscard]] const Node* if_contained_get_lowest_nonbranch(const std::string& str) const {
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

public:
    // __contains__ : Return true if str is in Trie, else return false
    [[nodiscard]] bool _contains_(const std::string& input) const {
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

    // push : Push str into our Trie
    void push(const std::string& input) {
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

    // remove
    void remove(const std::string& str) {
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

    /*
     Some powerful suggestion functions.
     If our Trie has {WHEREVER, WHETHER, WHICH, WHICHEVER, WHIFF, WHIG} and we search "WHICEVER",
     then it will return {WHICH, WHICHEVER} whose components has most 'closeness'. Here, the definition of
     'closeness' at the description of Trie::deepest_node_so_far() function.
     */
    [[nodiscard]] std::vector<std::string> get_suggestions(const std::string& input, const int MAX_SUGGESTIONS) const {
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

    // to_txt_data
    [[nodiscard]] std::string to_txt_data() const {
        std::string txt_holder;

        depth_first_search(txt_holder);

        return txt_holder;
    }

    // getter functions
    [[nodiscard]] auto get_preprocess() const {
        return preprocess;
    }
    [[nodiscard]] auto get_backprocess() const {
        return backprocess;
    }
};


#endif //OOPFINAL_TRIE_H
