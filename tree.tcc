template <typename K, typename V>
Tree<K, V>::Tree(std::filesystem::path const& path, std::size_t block_size)
	: allocator(path, block_size) {
}

template <typename K, typename V>
Tree<K, V>::Tree(std::filesystem::path const& path)
	: allocator(path) {}

template <typename K, typename V>
auto Tree<K, V>::search(K const& key) -> std::optional<V> {
	return get_root().search(key);
}

template <typename K, typename V>
void Tree<K, V>::insert(K const& key, V const& value) {
	if (get_root().is_full())
		allocator.replace_root();

	get_root().insert(key, value);
}

template <typename K, typename V>
void Tree<K, V>::print(std::ostream& out) {
	allocator.save_changes();
	allocator.get_root().print_all(out);
}
