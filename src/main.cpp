#include "rb_tree.hpp"
#include "rb_tree_io.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

namespace {

void to_lower(std::string& word) {
    for (char& c : word) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
}

void handle_insert(alg::RBTree& tree) {
    std::string word;
    uint64_t value = 0;

    if (!(std::cin >> word >> value)) {
        return;
    }

    to_lower(word);

    if (tree.insert(word, value)) {
        std::cout << "OK\n";
    } else {
        std::cout << "Exist\n";
    }
}

void handle_remove(alg::RBTree& tree) {
    std::string word;

    if (!(std::cin >> word)) {
        return;
    }

    to_lower(word);

    if (tree.remove(word)) {
        std::cout << "OK\n";
    } else {
        std::cout << "NoSuchWord\n";
    }
}

void handle_find(const alg::RBTree& tree, std::string& word) {
    to_lower(word);

    if (auto value = tree.find(word)) {
        std::cout << "OK: " << *value << '\n';
    } else {
        std::cout << "NoSuchWord\n";
    }
}

void handle_system_command(alg::RBTree& tree) {
    std::string cmd;
    std::string path;

    if (!(std::cin >> cmd >> path)) {
        return;
    }

    if (cmd == "Save") {
        alg::save(tree, path);
        std::cout << "OK\n";
    } else if (cmd == "Load") {
        alg::load(tree, path);
        std::cout << "OK\n";
    } else {
        std::cout << "ERROR: Unknown command\n";
    }
}

} // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    alg::RBTree tree;
    std::string token;

    while (std::cin >> token) {
        try {
            if (token == "+") {
                handle_insert(tree);
            } else if (token == "-") {
                handle_remove(tree);
            } else if (token == "!") {
                handle_system_command(tree);
            } else {
                handle_find(tree, token);
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << '\n';
        }
    }

    return 0;
}
