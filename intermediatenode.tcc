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
}

template <typename K, typename V>
void IntermediateNode<K, V>::serialize(std::ostream& out) const {
	Serializer::serialize(out, keys.size());
	
	for (auto& key: keys)
		Serializer::serialize(out, key);

	for (auto& index: indices)
		Serializer::serialize(out, index);

	Serializer::allocate<K>(out, get_max_keys() - keys.size());
	Serializer::allocate<Index>(out, get_max_keys() + 1 - indices.size());
	std::size_t data_size = keys.size()*sizeof(K) + indices.size()*sizeof(Index);
	Serializer::pad_out(out, get_block_size() - data_size);
}

template <typename K, typename V>
void IntermediateNode<K, V>::deserialize(std::istream& in) {
	std::size_t size;
	Serializer::deserialize(in, size);
	set_count(size);

	keys.clear();
	K key;
	for (int i = 0; i < size; ++i) {
		Serializer::deserialize(in, key);
		keys.push_back(key);
	}

	indices.clear();
	Index index;
	for (int i = 0; i < size + 1; ++i) {
		Serializer::deserialize(in, index);
		indices.push_back(index);
	}
}

template <typename K, typename V>
auto IntermediateNode<K, V>::search(K const& key) -> std::optional<V> {
	auto idx = Utility::binary_search(keys, key).first;
	if (key <= get_key(idx))
		return get_child(idx).search(key);
	else
		return get_child(idx + 1).search(key);
}

template <typename K, typename V>
auto IntermediateNode<K, V>::split_right() -> std::tuple<Node<K, V>, K> {
	std::size_t mid = get_count() / 2;
	K median = keys[mid];
	auto right = IntermediateNode(*this, mid, get_count());
	keys.resize(mid - 1);
	indices.resize(mid - 1);
	
	// WARNING! aren't there more indices than keys?
	// shouldn't this be indices.resize(mid) or indices.resize(mid+1)?

	return {right, median};
}

template <typename K, typename V>
void IntermediateNode<K, V>::insert(K const& key, V const& value) {
	int i = keys.size();
	while (keys[i] > key)
		--i;

	auto child = get_child(i);
	if (child.is_full())
		split_child(i);

	child.insert(key, value);
}

template <typename K, typename V>
void IntermediateNode<K, V>::split_child(std::size_t idx) {
	auto child = get_child(idx);
	auto [right_node, median] = child.split_right();
	auto right_node_idx = get_allocator().allocate(right_node);
	
	keys.push_back(median);
	for (std::size_t i = keys.size() - 1; i > right_node_idx; --i)
		std::swap(keys[i], keys[i - 1]);

	indices.push_back(child.get_index());
	for (std::size_t i = indices.size() - 1; i > right_node_idx + 1; --i)
		std::swap(indices[i], indices[i - 1]);

	set_modified();
}

template <typename K, typename V>
auto IntermediateNode<K, V>::get_child(std::size_t idx) -> Node<K, V>& {
	if (has_leaf_children)
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
