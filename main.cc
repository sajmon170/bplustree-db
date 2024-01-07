#include "tree.hh"
#include "triangle.hh"

namespace fs = std::filesystem;

auto main() -> int {
	auto tree = Tree<int, Triangle>(fs::path("mytree.db"), std::size_t(20));
}
