#pragma once

#include "iserializable.hh"

#include <filesystem>
#include <fstream>
#include <optional>
#include <tuple>

template <typename K, typename V>
class NodeAllocator;

template <typename K, typename V>
class Node : public ISerializable {
public:
	using Index = std::size_t;
	
private:
	NodeAllocator<K, V>& allocator;
	std::size_t level;
	std::fstream file;

	std::size_t degree;

	Index self;
	std::size_t count = 0;
	bool modified = false;

protected:
	inline void basic_init() {
		file = std::fstream(allocator.get_path(), std::ios::in
		                                        | std::ios::out
		                                        | std::ios::binary);
	}
	
	inline void set_count(std::size_t cnt) {
		count = cnt;
	}

	virtual auto split_right() -> std::tuple<Node, K> = 0;

	inline void set_modified() {
		modified = true;
	}

	inline void reset_modified() {
		modified = false;
	}

	inline void seek(std::size_t offset) {
		file.clear();
		file.seekg(offset);
	}

	inline void set_index(Index idx) {
		self = idx;
	}

	inline void increase_level() {
		++level;
	}

	inline void decrease_level() {
		--level;
	}

	inline auto get_allocator() -> NodeAllocator<K, V>& {
		return allocator;
	}

public:
	friend class NodeAllocator<K, V>;
	
	Node(NodeAllocator<K, V>&, std::size_t, std::size_t);
	Node(NodeAllocator<K, V>&, std::size_t, std::size_t, Index);
	Node(Node const&);

	virtual auto get_key(Index) const -> K = 0;
	virtual auto search(K const&) -> std::optional<V> = 0;
	virtual void insert(K const&, V const&) = 0;

	// from ISerializable
	inline auto size_on_disk() const -> std::size_t override {
		return get_block_size();
	}
	
	inline auto get_path() const -> std::filesystem::path {
		return allocator.get_path();
	}

	inline auto get_stream() -> std::fstream& {
		return file;
	}

	inline auto get_count() const -> std::size_t {
		return count;
	}

	inline auto get_max_keys() const -> std::size_t {
		return 2 * get_degree() - 1;
	}

	inline auto get_degree() const -> std::size_t {
		return degree;
	}

	inline auto get_block_size() const -> std::size_t {
		return allocator.get_block_size();
	}

	inline auto is_full() const -> bool {
		return count == 2*degree - 1;
	}

	inline auto was_modified() const -> bool {
		return modified;
	}

	inline auto get_index() const -> Index {
		return self;
	}

	inline void seek_index(Index idx) {
		seek(idx * size_on_disk());
		set_index(idx);
	}

	inline auto get_level() -> std::size_t {
		return level;
	}

	void read_disk();
	void overwrite();
};

#include "node.tcc"
