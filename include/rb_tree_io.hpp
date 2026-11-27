#pragma once

#include "rb_tree.hpp"

#include <string>

namespace alg {

void save(const RBTree& tree, const std::string& file_path);
void load(RBTree& tree, const std::string& file_path);

} // namespace alg
