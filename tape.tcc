template <typename T>
Tape<T>::Tape(std::filesystem::path path,
              std::uint64_t size,
              std::uint64_t offset)
	: size(size), offset(offset), path(path) {
	buffer.reserve(size);
}

template <typename T>
void Tape<T>::set_file(std::filesystem::path new_path, std::size_t offset) {
	close();
	set_offset(offset);
	set_path(new_path);
	open();
}

template <typename T>
auto Tape<T>::pop_front() -> T {
	return buffer[pos_beg++];
}

template <typename T>
auto Tape<T>::pop_back() -> T {
	T item = buffer.back();
	buffer.pop_back();
	return item;
}

template <typename T>
void Tape<T>::push(T const& value) {
	buffer.push_back(value);
}
