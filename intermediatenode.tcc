template <typename K, typename V>
IntermediateNode<K, V>::IntermediateNode(NodeAllocator<K, V>& allocator,
                                         std::size_t level,
                                         std::size_t degree)
	: Node<K, V>::Node(allocator, level, degree) {}

template <typename K, typename V>
IntermediateNode<K, V>::IntermediateNode(NodeAllocator<K, V>& allocator,
                                         std::size_t level,
                                         std::size_t degree,
                                         Index idx)
	: Node<K, V>::Node(allocator, level, degree, idx) {}

template <typename K, typename V>
IntermediateNode<K, V>::IntermediateNode(IntermediateNode<K, V> const& other,
                                         std::size_t beg,
                                         std::size_t end)
	: Node<K, V>::Node(other) {
	for (auto it = beg; it < end; ++it) {
		keys.push_back(other.keys[it]);
		indices.push_back(other.indices[it]);
	}

	indices.push_back(other.indices[end]);

	set_count(keys.size());
}

template <typename K, typename V>
void IntermediateNode<K, V>::serialize(std::ostream& out) const {
	Serializer::reset_count();

	std::size_t size = keys.size();
	Serializer::serialize(out, size);
	
	for (auto& key: keys)
		Serializer::serialize(out, key);

	for (auto& index: indices)
		Serializer::serialize(out, index);

	std::size_t written = Serializer::bytes_written();
	Serializer::pad_out(out, get_block_size() - written);
}

template <typename K, typename V>
void IntermediateNode<K, V>::deserialize(std::istream& in) {
	std::size_t size;
	Serializer::deserialize(in, size);
	set_count(size);

	keys.clear();
	K key;
	for (std::size_t i = 0; i < size; ++i) {
		Serializer::deserialize(in, key);
		keys.push_back(key);
	}

	indices.clear();
	Index index;
	for (std::size_t i = 0; i < size + 1; ++i) {
		Serializer::deserialize(in, index);
		indices.push_back(index);
	}
}

template <typename K, typename V>
auto IntermediateNode<K, V>::search(K const& key) -> std::optional<V> {
	auto key_no = Utility::binary_search(keys, key, [](K const& k) {
		return k;
	}).first;

	Index left_idx = indices[key_no];
	Index right_idx = indices[key_no + 1];
	Index child_idx = (key <= get_key(key_no))? left_idx : right_idx;

	return get_child(child_idx)->search(key);
}

template <typename K, typename V>
auto IntermediateNode<K, V>::split_right()
	-> std::tuple<std::unique_ptr<Node<K, V>>, K> {

	std::size_t mid = get_count() / 2;
	K median = keys[mid];
	auto right = std::make_unique<IntermediateNode>(*this, mid + 1, get_count());
	
	keys.resize(mid + 1);
	indices.resize(mid + 2);

	set_count(keys.size());
	set_modified();

	return { std::move(right), median };
}

template <typename K, typename V>
void IntermediateNode<K, V>::insert(K const& key, V const& value) {
	auto [key_no, _] = Utility::binary_search(keys, key, [](K const& k) {
		return k;
	});

	Index left_idx = indices[key_no];
	Index right_idx = indices[key_no + 1];
	Index child_idx = (key <= get_key(key_no))? left_idx : right_idx;

	auto& child = get_child(child_idx);

	if (child->is_full()) {
		split_child(child_idx);
		insert(key, value);
	}
	else {
		child->insert(key, value);
	}
}

template <typename K, typename V>
void IntermediateNode<K, V>::split_child(std::size_t idx) {
	auto& child = get_child(idx);
	
	auto [right_node, median] = child->split_right();
	auto right_node_idx = get_allocator().allocate(*right_node);
	//right_node->overwrite();

	keys.push_back(median);
	std::int64_t offset = keys.size() - 1;
	while (offset - 1 >= 0 && keys[offset] < keys[offset - 1]) {
		std::swap(keys[offset], keys[offset - 1]);
		--offset;
	}

	indices.push_back(right_node_idx);
	for (std::int64_t i = indices.size() - 1; i - 1 > offset; --i)
		std::swap(indices[i], indices[i - 1]);

	set_modified();
	increase_count();
}

template <typename K, typename V>
auto IntermediateNode<K, V>::get_child(std::size_t idx)
	-> std::unique_ptr<Node<K, V>>& {

	if (has_leaf_children())
		return get_allocator().cache_leaf(*this, idx);
	else
		return get_allocator().cache_intermediate(*this, idx);
}

template <typename K, typename V>
void IntermediateNode<K, V>::make_root(Index first) {
	keys.clear();
	indices.clear();
	indices.push_back(first);
}

template <typename K, typename V>
void IntermediateNode<K, V>::print(std::ostream& out) const {
	out << "{ ";
	out << indices.front() << " ";
	for (std::size_t i = 0; i < keys.size(); ++i) {
		out << "[" << keys[i] << "] " << indices[i + 1] << " ";
	}
	out << "} ";
}

template <typename K, typename V>
void IntermediateNode<K, V>::print_all(std::ostream& out) {
	std::size_t level = 1;
	out << "Level: " << level << "\n";
	print(out);
	out << "\n";
	std::queue<std::vector<Index>> intermediate;
	std::queue<std::vector<Index>> leaves;
	std::set<Index> visited;

	if (level == get_allocator().get_height() - 1)
		leaves.push(get_children());
	else
		intermediate.push(get_children());

	++level;
	std::size_t level_sz = 1;
	std::size_t next_sz;
	
	while (!intermediate.empty()) {
		next_sz = 0;

		out << "\nLevel: " << level << "\n";
		for (std::size_t i = 0; i < level_sz; ++i ) {
			auto& children = intermediate.front();

			for (auto& idx: children) {
				if (visited.count(idx))
					continue;

				visited.insert(idx);
				
				auto child = get_allocator().get_intermediate(idx);
				child.print(out);

				if (level == get_allocator().get_height() - 1)
					leaves.push(child.get_children());
				else {
					++next_sz;
					intermediate.push(child.get_children());
				}
			}

			intermediate.pop();
		}

		level_sz = next_sz;
		++level;
		out << std::endl;
	}

	out << std::endl;
	out << "Level: " << level << "\n";
	while (!leaves.empty()) {
		auto& children = leaves.front();

		for (auto& idx: children) {
			if (visited.count(idx))
				continue;

			visited.insert(idx);

			auto child = get_allocator().get_leaf(idx);
			child.print(out);
		}
		
		leaves.pop();
	}

	out << std::endl;
}
