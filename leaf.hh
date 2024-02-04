#pragma once

#include "node.hh"
#include "utils.hh"

#include <vector>
#include <utility>
#include <cmath>

template <typename K, typename V>
class Leaf : public Node<K, V> {
	using Index = typename Node<K, V>::Index;
	using Node<K, V>::get_allocator;
	using Node<K, V>::get_max_keys;
	using Node<K, V>::get_count;
	using Node<K, V>::set_count;
	using Node<K, V>::increase_count;
	using Node<K, V>::decrease_count;
	using Node<K, V>::set_modified;
	using Node<K, V>::get_block_size;
	using Node<K, V>::was_modified;
	using Node<K, V>::overwrite;

	Index left;
	std::vector<std::pair<K, V>> data;
	Index right;

public:
	using Node<K, V>::get_level;
	Leaf(NodeAllocator<K, V>&, std::size_t, std::size_t);
	Leaf(NodeAllocator<K, V>&, std::size_t, std::size_t, Index);
	Leaf(Leaf const&, std::size_t, std::size_t);

	~Leaf() {
		if (was_modified())
			overwrite();
	}

	// from ISerializable
	void serialize(std::ostream&) const override;
	void deserialize(std::istream&) override;

	// from Node
	inline auto get_key(Index idx) const -> K override {
		return data[idx].first;
	}
	void print(std::ostream&) const override;
	void print_all(std::ostream&) override;
	inline auto get_children() -> std::vector<Index> override {
		return {};
	}

	auto try_redistribute(std::optional<Index>, std::optional<Index>,
	                      K const& , V const&) -> std::optional<K> override;
	auto split_right() -> std::tuple<std::unique_ptr<Node<K, V>>, K> override;
	auto search(K const&) -> std::optional<V> override;
	void insert(K const&, V const&) override;

	static constexpr auto get_entry_size() -> std::size_t {
		return sizeof(std::pair<K, V>);
	}
};

#include "leaf.tcc"
