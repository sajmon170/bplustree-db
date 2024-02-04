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

	set_count(data.size());
}

template <typename K, typename V>
void Leaf<K, V>::serialize(std::ostream& out) const {
	Serializer::reset_count();
	Serializer::serialize(out, left);
	std::size_t size = data.size();
	Serializer::serialize(out, size);

	for (auto& [key, value]: data) {
		Serializer::serialize(out, key);
		Serializer::serialize(out, value);
	}
	
	Serializer::serialize(out, right);
	std::size_t written = Serializer::bytes_written();
	Serializer::pad_out(out, get_block_size() - written);
}

template <typename K, typename V>
void Leaf<K, V>::deserialize(std::istream& in) {
	Serializer::deserialize(in, left);
	std::size_t size;
	Serializer::deserialize(in, size);
	set_count(size);

	std::pair<K, V> pair;
	data.clear();
	for (std::size_t i = 0; i < size; i++) {
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
		return (*data_pair).second;
	else
		return {};
}

template <typename K, typename V>
auto Leaf<K, V>::try_redistribute(std::optional<Index> left_idx,
                                  std::optional<Index> right_idx,
                                  K const& key, V const& value)
	-> std::optional<K> {

	if (left_idx) {
		auto left = get_allocator().get_leaf(*left_idx, get_level());
		if (!left.is_full()) {
			insert(key, value);
			
			std::size_t med_idx = (left.get_count() + get_count()) / 2;
			K med = med_idx < left.get_count()
			      ? left.get_key(med_idx - 1)
			      : get_key(med_idx - left.get_count());

			while (left.get_count() < med_idx) {
				left.data.push_back(data.front());
				left.increase_count();

				// TODO: replace std::vector with std::deque
				for (std::size_t i = 0; i + 1 < data.size(); ++i)
					std::swap(data[i], data[i + 1]);

				data.pop_back();
				decrease_count();
			}
			
			overwrite();
			left.overwrite();
			return med;
		}
	}

	if (right_idx) {
		auto right = get_allocator().get_leaf(*right_idx, get_level());
		if (!right.is_full()) {
			insert(key, value);

			std::size_t med_idx = (right.get_count() + get_count()) / 2;
			K med = med_idx < get_count()
			      ? get_key(med_idx - 1)
			      : right.get_key(med_idx - get_count());

			while (right.get_count() < med_idx) {
				right.data.push_back(data.back());
				right.increase_count();

				data.pop_back();
				decrease_count();

				// TODO: replace std::vector with std::deque
				for (std::size_t i = right.data.size() - 1; i > 0; --i)
					std::swap(right.data[i], right.data[i - 1]);
			}
			
			overwrite();
			right.overwrite();
			return med;
		}
	}

	return { };
}

template <typename K, typename V>
auto Leaf<K, V>::split_right()
	-> std::tuple<std::unique_ptr<Node<K, V>>, K> {

	std::size_t mid = get_count() / 2;
	K median = data[mid].first;
	auto right = std::make_unique<Leaf>(*this, mid + 1, get_count());
	data.resize(mid + 1);

	set_count(data.size());
	set_modified();

	return { std::move(right), median };
}

template <typename K, typename V>
	void Leaf<K, V>::insert(K const& key, V const& value) {
	data.push_back({key, value});

	for (std::size_t i = data.size() - 1; i > 0 && get_key(i) < get_key(i-1); --i)
		std::swap(data[i], data[i - 1]);

	set_modified();
	increase_count();
}

template <typename K, typename V>
void Leaf<K, V>::print(std::ostream& out) const {
	out << "[ ";
	for (auto& [key, _]: data)
		out << key << " ";
	out << "] ";
}

template <typename K, typename V>
void Leaf<K, V>::print_all(std::ostream& out) {
	print(out);
}
