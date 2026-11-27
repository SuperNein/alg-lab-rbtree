#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace alg {

class RBTree {
public:
    RBTree() noexcept = default;
    ~RBTree();

    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;

    bool insert(const std::string& key, uint64_t value);
    bool remove(const std::string& key);
    std::optional<uint64_t> find(const std::string& key) const;

    void clear() noexcept;

    friend void swap(RBTree& lhs, RBTree& rhs) noexcept;
    friend bool save(const RBTree& tree, const std::string& file_path, std::string& error_msg);
    friend bool load(RBTree& tree, const std::string& file_path, std::string& error_msg);

private:
    enum class Color : uint8_t {
        red = 0,
        black = 1
    };

    struct Node {
        std::string key;
        uint64_t value = 0;
        Color color = Color::red;

        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        Node() = default;
        Node(std::string k, uint64_t v, Color c = Color::red) noexcept;

        bool is_left_child() const noexcept { return this == parent->left; }
        Node* grandparent() const noexcept { return parent->parent; }
        Node* uncle() const noexcept {
            Node* gp = grandparent();
            return parent->is_left_child() ? gp->right : gp->left;
        }
    };

    static Node nil_;
    Node* root_ = &nil_;
    std::size_t size_ = 0;

    void left_rotate(Node* pivot) noexcept;
    void right_rotate(Node* pivot) noexcept;

    void insert_fixup(Node* node) noexcept;
    void delete_fixup(Node* node) noexcept;
    
    void transplant(Node* old_node, Node* new_node) noexcept;
    Node* minimum(Node* subtree_root) const noexcept;
    Node* find_node(const std::string& key) const noexcept;
    void destroy_subtree(Node* node) noexcept;
};

} // namespace alg
