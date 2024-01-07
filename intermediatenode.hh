#pragma once

#include "node.hh"
#include "leaf.hh"
#include "utils.hh"

#include <vector>
#include <utility>
#include <memory>

template <typename K, typename V>
class IntermediateNode : public Node<K, V> {
	using Index = typename Node<K, V>::Index;
	using Node<K, V>::get_allocator;
	using Node<K, V>::get_path;
	using Node<K, V>::get_max_keys;
	using Node<K, V>::get_count;
	using Node<K, V>::set_count;
	using Node<K, V>::set_modified;
	using Node<K, V>::get_block_size;

	std::vector<K> keys;
	std::vector<Index> indices;
	bool has_leaf_children;

protected:
	// from Node
	auto split_right() -> std::tuple<Node<K, V>, K> override;
	
public:
	IntermediateNode(NodeAllocator<K, V>&, std::size_t, std::size_t);
	IntermediateNode(NodeAllocator<K, V>&, std::size_t, std::size_t, Index);
	IntermediateNode(IntermediateNode const&, std::size_t, std::size_t);

	// from ISerializable
	void serialize(std::ostream&) const override;
	void deserialize(std::istream&) override;

	// from Node
	inline auto get_key(Index idx) const -> K override {
		return keys[idx];
	}
	auto search(K const&) -> std::optional<V> override;
	void insert(K const&, V const&) override;

	void split_child(std::size_t);
	auto get_child(std::size_t) -> Node<K, V>&;
	
	inline void set_leaf_children() {
		has_leaf_children = true;
	}
	
	void make_root(Index);

	static constexpr auto get_entry_size() -> std::size_t {
		return sizeof(K) + sizeof(Index);
	}
};

#include "intermediatenode.tcc"
