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
    std::array<Node*, 26> next{};
    std::string* to_data;

public:
    Node(char _c, int _level);
    virtual ~Node();

    Node(const Node&) = delete;
    Node(const Node&&) = delete;
    Node& operator=(const Node&) = delete;

    // Add some nodes
    void put(char c);
    // Add a string at end node : The node with non-NULL string* to_data means that it is the end node.
    void put(const std::string& str);

    void remove(char c);
    void remove();

    // Get functions
    [[nodiscard]] char get_char() const;
    [[nodiscard]] Node* get_next(int idx) const;
    [[nodiscard]] int get_level() const;
    [[nodiscard]] int get_offspring_num() const;
    [[nodiscard]] std::string* get_strptr() const;
    // Get functions end

    // Traversal function
    [[nodiscard]] std::vector<std::string> traverse(int MAX_VEC_SIZE) const;

    // Depth-First-Search method.
protected:
    void depth_first_search(std::string& str) const;
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
    explicit Trie(std::function<std::string(std::string)> f, std::function<std::string(std::string)> g)
            : Node(35, 0), preprocess(std::move(f)), backprocess(std::move(g)) {}
            // Character number 35 means "#"; Initializes the top node
    explicit Trie(const std::vector<std::string>& vs,
         std::function<std::string(std::string)> f, std::function<std::string(std::string)> g)
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
    [[nodiscard]] const Node* deepest_node_so_far(const std::string& str) const;

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
    [[nodiscard]] const Node* if_contained_get_lowest_nonbranch(const std::string& str) const;

public:

    [[nodiscard]] bool _contains_(const std::string& input) const;
    void push(const std::string& input);
    void remove(const std::string& str);

    /*
     Some powerful suggestion functions.
     If our Trie has {WHEREVER, WHETHER, WHICH, WHICHEVER, WHIFF, WHIG} and we search "WHICEVER",
     then it will return {WHICH, WHICHEVER} whose components has most 'closeness'. Here, the definition of
     'closeness' at the description of Trie::deepest_node_so_far() function.
     */
    [[nodiscard]] std::vector<std::string> get_suggestions(const std::string& input, int MAX_SUGGESTIONS) const;

    // to_txt_data
    [[nodiscard]] std::string to_txt_data() const;

    // getter functions
    [[nodiscard]] auto get_preprocess() const;
    [[nodiscard]] auto get_backprocess() const;
};


#endif //OOPFINAL_TRIE_H
