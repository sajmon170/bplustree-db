#include "tree.hh"
#include "triangle.hh"

#include <iostream>
#include <set>
#include <random>
#include <filesystem>
#include <fstream>

#define BLOCK_SIZE 500

std::default_random_engine e;

auto mkrand(int key) -> Triangle {
	static std::uniform_real_distribution<double> d(1, 300);
	return {{d(e), d(e)}, {d(e), d(e)}, {d(e), d(e)}, key};
}

auto filltree(std::unique_ptr<Tree<int, Triangle>>& tree, int entries) {
	static std::uniform_int_distribution<int> d(1, 4000000);
	static std::set<int> used_keys;

	int key;
	while (entries--) {
		do key = d(e);
		while (used_keys.count(key) != 0);

		used_keys.insert(key);
		tree->insert(key, mkrand(key));
	}
}

void mktestfile(std::filesystem::path path, int count) {
	static std::uniform_int_distribution<int> d(1, 4000000);
	
	std::fstream file(path, std::ios::in | std::ios::out | std::ios::trunc);
	std::set<int> used_keys;

	int key;
	while (count--) {
		do key = d(e);
		while (used_keys.count(key) != 0);

		Triangle t = mkrand(key);
		file << key << " "
		     << t.getA().getX() << " " << t.getA().getY() << " "
		     << t.getB().getX() << " " << t.getB().getY() << " "
		     << t.getC().getX() << " " << t.getC().getY() << " ";
	}
}

void ldfile(std::filesystem::path path, std::unique_ptr<Tree<int, Triangle>>& tree) {
	std::fstream file(path, std::ios::in);

	Triangle t;
	int key;
	double x1, y1, x2, y2, x3, y3;
	while (file >> key >> x1 >> y1 >> x2 >> y2 >> x3 >> y3)
		tree->insert(key, {{x1, y1}, {x2, y2}, {x3, y3}, key});
}

auto main() -> int {
	auto tree = std::make_unique<Tree<int, Triangle>>("mytree.db", BLOCK_SIZE);

	std::string command;
	int arg;
	
	while (std::cin >> command) {
		if (command == "add") {
			std::cin >> arg;
			tree->insert(arg, mkrand(arg));
		}
		else if (command == "search") {
			std::cin >> arg;
			if (auto result = tree->search(arg))
				std::cout << *result << std::endl;
			else
				std::cout << "No record found." << std::endl;
		}
		else if (command == "print") {
			tree->print(std::cout);
			std::cout << std::endl;
		}
		else if (command == "fill") {
			std::cin >> arg;
			filltree(tree, arg);
			std::cout << "Filled the tree with " << arg
			          << " additional entries."  << std::endl;
		}
		else if (command == "exit") {
			return 0;
		}
		else if (command == "get_stats") {
			std::cout << "Reads: " << tree->get_reads() << "\n"
			          << "Writes: " << tree->get_writes() << std::endl;
		}
		else if (command == "reset_stats") {
			tree->reset_stats();
		}
		else if (command == "reset_tree") {
			tree.reset();
			tree = std::make_unique<Tree<int, Triangle>>("mytree.db", BLOCK_SIZE);
		}
		else if (command == "get_height") {
			std::cout << tree->get_height() << std::endl;
		}
		else if (command == "mkfile") {
			std::cin >> arg;
			mktestfile("test.txt", arg);
		}
		else if (command == "ldfile") {
			ldfile("test.txt", tree);
		}
		else {
			std::cout << "Unknown command." << std::endl;
		}
	}
}
