#pragma once

#include "node.hh"
#include "leaf.hh"
#include "utils.hh"

#include <vector>
#include <queue>
#include <set>
#include <utility>
#include <memory>
#include <optional>

template <typename K, typename V>
class IntermediateNode : public Node<K, V> {
	using Index = typename Node<K, V>::Index;
	using Node<K, V>::get_allocator;
	using Node<K, V>::get_path;
	using Node<K, V>::get_max_keys;
	using Node<K, V>::get_count;
	using Node<K, V>::set_count;
	using Node<K, V>::increase_count;
	using Node<K, V>::set_modified;
	using Node<K, V>::get_block_size;
	using Node<K, V>::has_leaf_children;
	using Node<K, V>::was_modified;
	using Node<K, V>::overwrite;
	using Node<K, V>::get_index;

	std::vector<K> keys;
	std::vector<Index> indices;
	
public:
	IntermediateNode(NodeAllocator<K, V>&, std::size_t, std::size_t);
	IntermediateNode(NodeAllocator<K, V>&, std::size_t, std::size_t, Index);
	IntermediateNode(IntermediateNode const&, std::size_t, std::size_t);
	~IntermediateNode() {
		if (was_modified())
			overwrite();
	}

	// from ISerializable
	void serialize(std::ostream&) const override;
	void deserialize(std::istream&) override;

	// from Node
	inline auto get_key(Index idx) const -> K override {
		return keys[idx];
	}
	auto search(K const&) -> std::optional<V> override;
	void insert(K const&, V const&) override;
	auto split_right() -> std::tuple<std::unique_ptr<Node<K, V>>, K> override;
	void print(std::ostream&) const override;
	void print_all(std::ostream&) override;
	inline auto get_children() -> std::vector<Index> override {
		return indices;
	}
	
	void split_child(std::size_t);
	auto get_child(std::size_t) -> std::unique_ptr<Node<K, V>>&;
	
	void make_root(Index);

	static constexpr auto get_entry_size() -> std::size_t {
		return sizeof(K) + sizeof(Index);
	}
};

#include "intermediatenode.tcc"
