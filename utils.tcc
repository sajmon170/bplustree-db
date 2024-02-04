namespace Utility {
	template <typename Container>
	using SearchResult = std::pair<std::size_t,
	                               std::optional<typename Container::value_type>>;

	template <typename Container, typename K, typename Func>
	auto binary_search(Container const& container, K const& key, Func extract)
		-> SearchResult<Container> {
		std::int64_t l = 0;
		std::int64_t r = container.size()-1;
		std::int64_t idx;
		
		while (l <= r) {
			idx = (l + r) / 2;
			if (key < extract(container[idx]))
				r = idx - 1;
			else if (key > extract(container[idx]))
				l = idx + 1;
			else
				return SearchResult<Container>(idx, container[idx]);
		}

		return SearchResult<Container>(idx, {});
	}
}
