#include "rb_tree_io.hpp"

#include <cerrno>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <vector>

namespace alg {

namespace {

constexpr uint32_t FILE_MAGIC = 0x52424454;
constexpr uint16_t FORMAT_VERSION = 1;
constexpr uint16_t MAX_KEY_LEN = 256;

[[noreturn]] void throw_io_error(const char* fallback) {
    if (errno != 0) {
        throw std::runtime_error(std::system_category().message(errno));
    }
    throw std::runtime_error(fallback);
}

bool is_valid_key(const std::string& key) {
    if (key.empty() || key.size() > MAX_KEY_LEN) {
        return false;
    }

    for (char c : key) {
        if (c < 'a' || c > 'z') {
            return false;
        }
    }

    return true;
}

void write_header(std::ofstream& out, uint64_t count) {
    out.write(reinterpret_cast<const char*>(&FILE_MAGIC), sizeof(FILE_MAGIC));
    out.write(reinterpret_cast<const char*>(&FORMAT_VERSION), sizeof(FORMAT_VERSION));
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    if (!out) {
        throw_io_error("Failed to write file header");
    }
}

uint64_t read_header(std::ifstream& in) {
    uint32_t magic = 0;
    uint16_t version = 0;
    uint64_t count = 0;

    in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    in.read(reinterpret_cast<char*>(&version), sizeof(version));
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (!in || magic != FILE_MAGIC || version != FORMAT_VERSION) {
        throw std::runtime_error("Invalid file format");
    }

    return count;
}

void write_record(std::ofstream& out, const std::string& key, uint64_t value) {
    uint16_t len = static_cast<uint16_t>(key.size());

    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(key.data(), len);
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));

    if (!out) {
        throw_io_error("Failed to write record");
    }
}

void read_record(std::ifstream& in, std::string& key, uint64_t& value) {
    uint16_t len = 0;

    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (!in || len == 0 || len > MAX_KEY_LEN) {
        throw std::runtime_error("Invalid file format");
    }

    key.resize(len);
    in.read(key.data(), len);
    in.read(reinterpret_cast<char*>(&value), sizeof(value));

    if (!in) {
        throw std::runtime_error("Unexpected end of file");
    }

    if (!is_valid_key(key)) {
        throw std::runtime_error("Invalid key in file");
    }
}

} // namespace

void save(const RBTree& tree, const std::string& file_path) {
    errno = 0;
    std::ofstream out(file_path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw_io_error("Failed to open file for writing");
    }

    write_header(out, static_cast<uint64_t>(tree.size_));

    if (tree.root_ != &RBTree::nil_) {
        std::vector<const RBTree::Node*> stack;
        stack.push_back(tree.root_);

        while (!stack.empty()) {
            const RBTree::Node* current = stack.back();
            stack.pop_back();

            write_record(out, current->key, current->value);

            if (current->right != &RBTree::nil_) {
                stack.push_back(current->right);
            }

            if (current->left != &RBTree::nil_) {
                stack.push_back(current->left);
            }
        }
    }

    errno = 0;
    out.flush();
    if (!out) {
        throw_io_error("Failed to flush file");
    }
}

void load(RBTree& tree, const std::string& file_path) {
    errno = 0;
    std::ifstream in(file_path, std::ios::binary);
    if (!in) {
        throw_io_error("Failed to open file for reading");
    }

    uint64_t count = read_header(in);

    RBTree temp;
    uint64_t value = 0;

    for (uint64_t i = 0; i < count; ++i) {
        std::string key;
        read_record(in, key, value);

        if (!temp.insert(std::move(key), value)) {
            throw std::runtime_error("Duplicate key in file");
        }
    }

    if (in.peek() != std::char_traits<char>::eof()) {
        throw std::runtime_error("Trailing data in file");
    }

    swap(tree, temp);
}

} // namespace alg
