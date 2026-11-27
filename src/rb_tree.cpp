#include "rb_tree.hpp"

#include <utility>

namespace alg {

RBTree::Node RBTree::nil_{"", 0, Color::black};

RBTree::Node::Node(std::string k, uint64_t v, Color c) noexcept
    : key(std::move(k)),
      value(v),
      color(c),
      left(&RBTree::nil_),
      right(&RBTree::nil_),
      parent(&RBTree::nil_) {}

RBTree::~RBTree() {
    clear();
}

void RBTree::clear() noexcept {
    destroy_subtree(root_);
    root_ = &nil_;
    size_ = 0;
}

void swap(RBTree& lhs, RBTree& rhs) noexcept {
    std::swap(lhs.root_, rhs.root_);
    std::swap(lhs.size_, rhs.size_);
}

void RBTree::destroy_subtree(Node* node) noexcept {
    if (node == &nil_ || node == nullptr) {
        return;
    }
    destroy_subtree(node->left);
    destroy_subtree(node->right);
    delete node;
}

RBTree::Node* RBTree::find_node(const std::string& key) const noexcept {
    Node* current = root_;
    while (current != &nil_) {
        if (key == current->key) {
            return current;
        }
        current = (key < current->key) ? current->left : current->right;
    }
    return &nil_;
}

std::optional<uint64_t> RBTree::find(const std::string& key) const {
    Node* node = find_node(key);
    if (node == &nil_) {
        return std::nullopt;
    }
    return node->value;
}

void RBTree::left_rotate(Node* pivot) noexcept {
    Node* child = pivot->right;
    pivot->right = child->left;

    if (child->left != &nil_) {
        child->left->parent = pivot;
    }

    child->parent = pivot->parent;

    if (pivot->parent == &nil_) {
        root_ = child;
    } else if (pivot->is_left_child()) {
        pivot->parent->left = child;
    } else {
        pivot->parent->right = child;
    }

    child->left = pivot;
    pivot->parent = child;
}

void RBTree::right_rotate(Node* pivot) noexcept {
    Node* child = pivot->left;
    pivot->left = child->right;

    if (child->right != &nil_) {
        child->right->parent = pivot;
    }

    child->parent = pivot->parent;

    if (pivot->parent == &nil_) {
        root_ = child;
    } else if (pivot->is_left_child()) {
        pivot->parent->left = child;
    } else {
        pivot->parent->right = child;
    }

    child->right = pivot;
    pivot->parent = child;
}

bool RBTree::insert(const std::string& key, uint64_t value) {
    Node* parent = &nil_;
    Node* current = root_;

    while (current != &nil_) {
        parent = current;
        if (key == current->key) {
            return false;
        }

        current = (key < current->key) ? current->left : current->right;
    }

    Node* node = new Node(key, value, Color::red);
    node->parent = parent;

    if (parent == &nil_) {
        root_ = node;
    } else if (node->key < parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    insert_fixup(node);
    ++size_;

    return true;
}

void RBTree::insert_fixup(Node* node) noexcept {
    while (node->parent->color == Color::red) {
        Node* gp = node->grandparent();
        Node* u = node->uncle();

        if (u->color == Color::red) {
            node->parent->color = Color::black;
            u->color = Color::black;
            gp->color = Color::red;

            node = gp;
        } else {
            if (node->parent->is_left_child()) {
                if (!node->is_left_child()) {
                    node = node->parent;
                    left_rotate(node);
                }

                node->parent->color = Color::black;
                node->grandparent()->color = Color::red;

                right_rotate(node->grandparent());
            } else {
                if (node->is_left_child()) {
                    node = node->parent;
                    right_rotate(node);
                }

                node->parent->color = Color::black;
                node->grandparent()->color = Color::red;

                left_rotate(node->grandparent());
            }
        }
    }
    root_->color = Color::black;
}

void RBTree::transplant(Node* old_node, Node* new_node) noexcept {
    if (old_node->parent == &nil_) {
        root_ = new_node;
    } else if (old_node->is_left_child()) {
        old_node->parent->left = new_node;
    } else {
        old_node->parent->right = new_node;
    }

    new_node->parent = old_node->parent;
}

RBTree::Node* RBTree::minimum(Node* subtree_root) const noexcept {
    while (subtree_root->left != &nil_) {
        subtree_root = subtree_root->left;
    }

    return subtree_root;
}

bool RBTree::remove(const std::string& key) {
    Node* target = find_node(key);
    if (target == &nil_) {
        return false;
    }

    Node* successor = target;
    Color original_color = successor->color;
    Node* child = &nil_;

    if (target->left == &nil_) {
        child = target->right;
        transplant(target, target->right);
    } else if (target->right == &nil_) {
        child = target->left;
        transplant(target, target->left);
    } else {
        successor = minimum(target->right);
        original_color = successor->color;
        child = successor->right;

        if (successor->parent == target) {
            child->parent = successor;
        } else {
            transplant(successor, successor->right);
            successor->right = target->right;
            successor->right->parent = successor;
        }

        transplant(target, successor);

        successor->left = target->left;
        successor->left->parent = successor;
        successor->color = target->color;
    }

    delete target;

    if (original_color == Color::black) {
        delete_fixup(child);
    }

    --size_;

    return true;
}

void RBTree::delete_fixup(Node* node) noexcept {
    while (node != root_ && node->color == Color::black) {
        if (node->is_left_child()) {
            Node* sibling = node->parent->right;

            if (sibling->color == Color::red) {
                sibling->color = Color::black;
                node->parent->color = Color::red;

                left_rotate(node->parent);
                sibling = node->parent->right;
            }

            if (sibling->left->color == Color::black && sibling->right->color == Color::black) {
                sibling->color = Color::red;
                node = node->parent;
            } else {
                if (sibling->right->color == Color::black) {
                    sibling->left->color = Color::black;
                    sibling->color = Color::red;

                    right_rotate(sibling);

                    sibling = node->parent->right;
                }
                sibling->color = node->parent->color;
                node->parent->color = Color::black;
                sibling->right->color = Color::black;

                left_rotate(node->parent);

                node = root_;
            }
        } else {
            Node* sibling = node->parent->left;

            if (sibling->color == Color::red) {
                sibling->color = Color::black;
                node->parent->color = Color::red;

                right_rotate(node->parent);

                sibling = node->parent->left;
            }

            if (sibling->right->color == Color::black && sibling->left->color == Color::black) {
                sibling->color = Color::red;
                node = node->parent;
            } else {
                if (sibling->left->color == Color::black) {
                    sibling->right->color = Color::black;
                    sibling->color = Color::red;

                    left_rotate(sibling);

                    sibling = node->parent->left;
                }

                sibling->color = node->parent->color;
                node->parent->color = Color::black;
                sibling->left->color = Color::black;

                right_rotate(node->parent);

                node = root_;
            }
        }
    }

    node->color = Color::black;
}

} // namespace alg
