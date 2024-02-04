#pragma once

#include <utility>
#include <optional>
#include <functional>

namespace Utility {
	template <typename Container, typename K, typename Func>
	auto binary_search(Container const& container, K const&, Func fn)
		-> std::pair<std::size_t, std::optional<typename Container::value_type>>;
}

#include "utils.tcc"
