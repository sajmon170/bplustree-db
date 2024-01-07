#pragma once

#include <utility>
#include <optional>

namespace Utility {
	template <typename T>
	auto identity_fn = [](T const& x) {
		return x;
	};

	template <typename Container, typename K, typename V, typename KeyFn>
	auto binary_search(Container const&, K const&, KeyFn fn = identity_fn<K>)
		-> std::pair<std::size_t, std::optional<V>>;
}

#include "utils.tcc"
