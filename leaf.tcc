template <typename K, typename V>
Leaf<K, V>::Leaf(NodeAllocator<K ,V>& allocator, std::size_t level,
                 std::size_t degree)
	: Node<K, V>::Node(allocator, level, degree) {}

template <typename K, typename V>
Leaf<K, V>::Leaf(NodeAllocator<K, V>& allocator, std::size_t level,
                 std::size_t degree, Index idx)
	: Node<K, V>::Node(allocator, level, degree, idx) {}

template <typename K, typename V>
Leaf<K, V>::Leaf(Leaf<K, V> const& other, std::size_t beg, std::size_t end)
	: Node<K, V>::Node(other) {
	left = other.left;
	right = other.right;
	for (auto it = beg; it < end; ++it)
		data.push_back(other.data[it]);
}

template <typename K, typename V>
void Leaf<K, V>::serialize(std::ostream& out) const {
	Serializer::serialize(out, left);

	Serializer::serialize(out, data.size());
	for (auto& [key, value]: data) {
		Serializer::serialize(out, key);
		Serializer::serialize(out, value);
	}
	
	Serializer::serialize(out, right);
	Serializer::allocate<std::pair<K, V>>(out, get_max_keys() - data.size());
	std::size_t data_size = 2*sizeof(Index) + data.size() * sizeof(data[0]);
	Serializer::pad_out(out, get_block_size() - data_size);
}

template <typename K, typename V>
void Leaf<K, V>::deserialize(std::istream& in) {
	Serializer::deserialize(in, left);
	std::size_t size;
	Serializer::deserialize(in, size);
	set_count(size);

	std::pair<K, V> pair;
	data.clear();
	for (int i = 0; i < size; i++) {
		Serializer::deserialize(in, pair.first);
		Serializer::deserialize(in, pair.second);
		data.push_back(pair);
	}

	Serializer::deserialize(in, right);
}

template <typename K, typename V>
	auto Leaf<K, V>::search(K const& key) -> std::optional<V> {
	auto [_, data_pair] = Utility::binary_search(data, key, [](auto const& pair) {
		return pair.first;
	});

	if (data_pair)
		return std::get<V>(data_pair);
	else
		return {};
}


template <typename K, typename V>
	auto Leaf<K, V>::split_right() -> std::tuple<Node<K, V>, K> {
	std::size_t mid = get_count() / 2;
	K median = data[mid].first;
	auto right = Leaf(*this, mid, get_count());
	data.resize(mid - 1);

	return {right, median};
}

template <typename K, typename V>
	void Leaf<K, V>::insert(K const& key, V const& value) {
	data.push_back({key, value});

	for (std::size_t i = data.size() - 1; get_key(i) > key; --i)
		std::swap(data[i], data[i - 1]);

	set_modified();
}

