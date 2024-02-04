template <typename K, typename V>
Node<K, V>::Node(NodeAllocator<K, V>& allocator, std::size_t level,
                 std::size_t degree)
	: allocator(allocator), level(level), degree(degree) {
	basic_init();
}

template <typename K, typename V>
Node<K, V>::Node(NodeAllocator<K, V>& allocator, std::size_t level,
                 std::size_t degree, Index idx)
	: self(idx), allocator(allocator), level(level), degree(degree) {
	basic_init();
	read_disk();
}

template <typename K, typename V>
Node<K, V>::Node(Node<K, V> const& other)
	: allocator(other.allocator), level(other.level), degree(other.degree),
	  self(other.self) {
	basic_init();
	leaf_children = other.leaf_children;
}

template <typename K, typename V>
void Node<K, V>::overwrite() {
	seek_index(self);
	serialize(file);
	seek_index(self);
	reset_modified();
}

template <typename K, typename V>
void Node<K, V>::read_disk() {
	seek_index(self);
	deserialize(file);
	seek_index(self);
	reset_modified();
}
