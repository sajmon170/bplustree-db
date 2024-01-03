template <typename T>
void OutputTape<T>::save() {
	for (auto& record: get_buffer())
		record.serialize(get_stream());

	++write_count;
	Tape<T>::clear();
}

template <typename T>
void OutputTape<T>::save(Tape<T>& other) {
	Tape<T>::clear();
	Tape<T>::swap_with(other);
	//get_buffer().swap(other.get_buffer());
	save();
}

template <typename T>
void OutputTape<T>::push(T const& value) {
	Tape<T>::push(value);
	if (get_buffer().size() == get_size())
		save();
}

template <typename T>
void OutputTape<T>::open() {
	get_stream().open(get_path(), std::ios::out
	                            | std::ios::binary
	                            | std::ios::trunc);
	get_stream().seekg(get_offset());
}
