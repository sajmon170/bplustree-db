#pragma once

#include "node.hh"
#include "utils.hh"

#include <vector>
#include <utility>

template <typename K, typename V>
class Leaf : public Node<K, V> {
	using Index = typename Node<K, V>::Index;
	using Node<K, V>::get_max_keys;
	using Node<K, V>::get_count;
	using Node<K, V>::set_count;
	using Node<K, V>::set_modified;
	using Node<K, V>::get_block_size;

	Index left;
	std::vector<std::pair<K, V>> data;
	Index right;

public:
	Leaf(NodeAllocator<K, V>&, std::size_t, std::size_t);
	Leaf(NodeAllocator<K, V>&, std::size_t, std::size_t, Index);
	Leaf(Leaf const&, std::size_t, std::size_t);

	// from ISerializable
	void serialize(std::ostream&) const override;
	void deserialize(std::istream&) override;

	// from Node
	inline auto get_key(Index idx) const -> K override {
		return data[idx].first;
	}

	auto split_right() -> std::tuple<Node<K, V>, K> override;
	auto search(K const&) -> std::optional<V> override;
	void insert(K const&, V const&) override;

	static constexpr auto get_entry_size() -> std::size_t {
		return sizeof(std::pair<K, V>);
	}
};

#include "leaf.tcc"
