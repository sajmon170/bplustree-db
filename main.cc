#include "tree.hh"
#include "triangle.hh"

#include <iostream>
#include <set>
#include <random>

void nbr() {}

auto main() -> int {
	auto tree = Tree<int, Triangle>("mytree.db", 500);

	std::default_random_engine e;
	std::uniform_int_distribution<int> d(1, 300);
	std::set<int> generated;
	
	int records = 108;
	int randnum;

	for (int i = 0; i < records; ++i) {
		do randnum = d(e);
		while (generated.count(randnum) != 0);

		if (i == 108 - 1) {
			std::cout << "Inserting: " << randnum << std::endl;
			nbr();
		}

		tree.insert(randnum, {{1, 2}, {3, 4}, {5, 6}, randnum});
		generated.insert(randnum);

		if (i == 107 - 1) {
			tree.print(std::cout);
			std::cout << "===========================" << std::endl;
		}
	}

	tree.print(std::cout);
}
