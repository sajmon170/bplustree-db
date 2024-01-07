template <typename K, typename V>
NodeAllocator<K, V>::NodeAllocator(std::filesystem::path const& path,
                                   std::size_t block_size)
	: path(path), block_size(block_size) {
	auto max_keys = block_size / IntermediateNode<K, V>::get_entry_size();
	if (max_keys % 2 == 0)
		--max_keys;
	node_degree = (max_keys + 1) / 2;

	auto max_records = block_size / IntermediateNode<K, V>::get_entry_size();
	if (max_records % 2 == 0)
		--max_records;
	leaf_degree = (max_records + 1) / 2;
	
	file = std::fstream(path, std::ios::in
	                        | std::ios::out
	                        | std::ios::binary
	                        | std::ios::trunc);
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
auto NodeAllocator<K, V>::allocate(Node<K, V> const& node) -> Index {
	Index address;
	if (!empty_blocks.empty()) {
		address = empty_blocks.back();
		empty_blocks.pop_back();
	}
	else {
		++node_count;
		address = node_count;
	}
	
	node.seek_index(address);
	node.overwrite();

	return address;
}

template <typename K, typename V>
void NodeAllocator<K, V>::defragment() {
	// todo: implement this
	while (!empty_blocks.empty());
}

template <typename K, typename V>
void NodeAllocator<K, V>::replace_root() {
	auto new_root = IntermediateNode<K, V>(*this, 0);
	new_root.make_root(root_idx);
	new_root.split(0);
	root_idx = allocate(new_root);

	for (auto& node: nodes)
		node->increase_level();
	
	nodes.push_front(new_root);
	++tree_height;
}

template <typename K, typename V>
auto NodeAllocator<K, V>::cache_leaf(IntermediateNode<K, V>& node, Index idx)
	-> Leaf<K, V>& {
	// leaves always reside on the last level
	if (nodes.back().get_index != idx) {
		nodes.pop_back();
		nodes.push_back(std::make_unique<Leaf<K, V>>(*this, nodes.size() + 1, idx));
	}
	
	return *nodes.back();
}

template <typename K, typename V>
auto NodeAllocator<K, V>::cache_intermediate(IntermediateNode<K, V>& node, Index idx)
	-> IntermediateNode<K, V>& {
	auto make_new = [&]() {
		nodes.push_back(std::make_unique<IntermediateNode<K, V>>(
			*this, node.get_level() + 1, idx
		));

		if (nodes.get_level() + 1 == tree_height - 1)
			static_cast<IntermediateNode<K, V>>(nodes.back())->set_leaf_children();
	};
	
	if (nodes.size() == node.get_level()) {
		make_new();
		return *nodes.back();
	}
	else if (nodes[node.get_level() + 1]->get_index() == idx) {
		return *nodes[node.get_level() + 1];
	}
	else {
		if (nodes[node.get_level() + 1]->was_changed())
			nodes[node.get_level() + 1]->overwrite();

		nodes.resize(node.get_level());
		
		make_new();
		return *nodes.back();
	}
}
