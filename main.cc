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
	
	int records = 14;
	int randnum;

	std::cout << "Generated keys: \n";
	for (int i = 0; i < records; ++i) {
		do randnum = d(e);
		while (generated.count(randnum) != 0);

		tree.insert(randnum, {{1, 2}, {3, 4}, {5, 6}, randnum});
		generated.insert(randnum);

		std::cout << "==========================================" << std::endl;
		std::cout << "Inserting: " << randnum << "\n";
		tree.print(std::cout);
		std::cout << "\n";

	}

	std::cout << "\n\n";
	tree.print(std::cout);
}
