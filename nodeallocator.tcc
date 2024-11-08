#include <iostream>

template <typename K, typename V>
NodeAllocator<K, V>::NodeAllocator(std::filesystem::path const& path,
                                   std::size_t block_size)
	: path(path), block_size(block_size) {
	auto max_keys = block_size / IntermediateNode<K, V>::get_entry_size();
	if (max_keys % 2 == 0)
		--max_keys;
	//node_degree = (max_keys + 1) / 2;
	node_degree = 2;

	auto max_records = block_size / Leaf<K, V>::get_entry_size();
	if (max_records % 2 == 0)
		--max_records;
	//leaf_degree = (max_records + 1) / 2;
	leaf_degree = 2;

	nodes.emplace_back(std::make_unique<Leaf<K, V>>(*this, 0, leaf_degree));
	
	file = std::fstream(path, std::ios::in
	                        | std::ios::out
	                        | std::ios::binary
	                        | std::ios::trunc);

	allocate(*nodes.back());
}

template <typename K, typename V>
NodeAllocator<K, V>::NodeAllocator(std::filesystem::path const&) {
	file = std::fstream(path, std::ios::in
	                        | std::ios::out
	                        | std::ios::binary);
	deserialize(file);
}

template <typename K, typename V>
void NodeAllocator<K, V>::serialize(std::ostream& out) const {
	Serializer::serialize(out, tree_height);
	Serializer::serialize(out, node_degree);
	Serializer::serialize(out, leaf_degree);
	Serializer::serialize(out, block_size);
	Serializer::serialize(out, root_idx);
	Serializer::serialize(out, node_count);
	Serializer::serialize(out, tree_height);
	//todo: serialize empty_blocks
}

template <typename K, typename V>
void NodeAllocator<K, V>::deserialize(std::istream& in) {
	Serializer::deserialize(in, tree_height);
	Serializer::deserialize(in, node_degree);
	Serializer::deserialize(in, leaf_degree);
	Serializer::deserialize(in, block_size);
	Serializer::deserialize(in, root_idx);
	Serializer::deserialize(in, node_count);
	Serializer::deserialize(in, tree_height);
	//todo: deserialize empty_blocks
}

template <typename K, typename V>
auto NodeAllocator<K, V>::size_on_disk() const -> std::size_t {
	return sizeof(get_height())
	     + sizeof(node_degree)
	     + sizeof(leaf_degree)
	     + sizeof(block_size)
	     + sizeof(root_idx)
	     + sizeof(node_count)
	     + sizeof(tree_height);
	//   + sizeof(Index) * empty_blocks.size();
}

template <typename K, typename V>
auto NodeAllocator<K, V>::allocate(Node<K, V>& node) -> Index {
	Index address;
	if (!empty_blocks.empty()) {
		address = empty_blocks.back();
		empty_blocks.pop_back();
	}
	else {
		address = node_count++;
	}
	
	node.set_index(address);
	node.overwrite();

	return address;
}

template <typename K, typename V>
void NodeAllocator<K, V>::replace_root() {
	for (auto& node: nodes)
		node->increase_level();

	nodes.emplace_front(
		std::make_unique<IntermediateNode<K, V>>(*this, 1, node_degree)
	);

	if (tree_height == 1)
		nodes.front()->set_leaf_children();
	
	++tree_height;
	
	auto& new_root = static_cast<IntermediateNode<K, V>&>(*nodes.front());
	Index old_root = root_idx;
	new_root.make_root(old_root);
	root_idx = allocate(new_root);
	new_root.split_child(old_root, { });
}

template <typename K, typename V>
auto NodeAllocator<K, V>::cache_leaf(IntermediateNode<K, V>& node, Index idx)
	-> std::unique_ptr<Node<K, V>>& {
	auto make_new = [&]() {
		nodes.emplace_back(std::make_unique<Leaf<K, V>>(
			*this, tree_height, leaf_degree
		));

		nodes.back()->set_index(idx);
		nodes.back()->read_disk();
	};
	
	// Leaves always reside on the last level.
	// The last level doesn't have to be present in buffered memory
	// if the intermediate node before it was replaced.
	if (nodes.size() == tree_height - 1) {
		make_new();
	}
	else if (nodes.back()->get_index() != idx) {
		if (nodes.back()->was_modified())
			nodes.back()->overwrite();
		
		nodes.pop_back();
		make_new();
	}
	
	return nodes.back();
}

template <typename K, typename V>
auto NodeAllocator<K, V>::cache_intermediate(IntermediateNode<K, V>& parent,
                                             Index child_idx)
	-> std::unique_ptr<Node<K, V>>& {
	auto make_new = [&]() {
		nodes.emplace_back(std::make_unique<IntermediateNode<K, V>>(
			*this, parent.get_level() + 1, node_degree
		));

		nodes.back()->set_index(child_idx);
		nodes.back()->read_disk();

		if (parent.get_level() == tree_height - 2)
			nodes[parent.get_level()]->set_leaf_children();
	};
	
	if (nodes.size() == parent.get_level()) {
		make_new();
		return nodes.back();
	}
	else if (nodes[parent.get_level()]->get_index() == child_idx) {
		return nodes[parent.get_level()];
	}
	else {
		if (nodes[parent.get_level()]->was_modified())
			nodes[parent.get_level()]->overwrite();

		nodes.resize(parent.get_level());
		
		make_new();
		return nodes.back();
	}
}

template <typename K, typename V>
auto NodeAllocator<K, V>::get_leaf(Index child_idx, std::size_t level)
	-> Leaf<K, V> {
	auto leaf = Leaf<K, V>(*this, level, leaf_degree);
	leaf.set_index(child_idx);
	leaf.read_disk();
	
	return leaf;
}

template <typename K, typename V>
auto NodeAllocator<K, V>::get_intermediate(Index child_idx, std::size_t level)
	-> IntermediateNode<K, V> {
	auto node = IntermediateNode<K, V>(*this, level, node_degree);
	node.set_index(child_idx);
	node.read_disk();

	return node;
}

template <typename K, typename V>
void NodeAllocator<K, V>::debug(std::ostream& out) {
	for (auto& node: nodes) {
		node->print(out);
		out << "\n";
	}
}

template <typename K, typename V>
void NodeAllocator<K, V>::save_changes() {
	for (auto& node: nodes) {
		if (node->was_modified())
			node->overwrite();
	}
}
