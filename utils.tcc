namespace Utility {
	template <typename Container, typename K, typename V, typename KeyFn>
	auto binary_search(Container const& container, K const& key, KeyFn extract)
		-> std::pair<std::size_t, std::optional<V>> {
		std::size_t l = 0;
		std::size_t r = container.size()-1;
		std::size_t idx;
		
		while (l <= r) {
			idx = (l + r) / 2;
			if (key < extract(container[idx]))
				r = idx - 1;
			else if (key > extract(container[idx]))
				l = idx + 1;
			else
				return std::pair(idx, container[idx]);
		}

		return std::pair<std::size_t, std::optional<V>>(idx, {});
	}
}
