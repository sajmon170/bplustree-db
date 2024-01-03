template <typename T>
auto InputTape<T>::fill_buffer() -> bool {
	Tape<T>::clear();

	if (is_limited && refill_limit == 0) {
		lock();
		return false;
	}

	if (is_limited)
		--refill_limit;

	if (logging_enabled)
		++read_count;

	bool state = get_stream().good();

	T record;
	int it = get_size();
	while (it > 0 && get_stream() >> record) {
		Tape<T>::push(record);
		--it;
	}

	return state;
}

template <typename T>
auto InputTape<T>::pop_front() -> T {
	T value = Tape<T>::pop_front();

	if (Tape<T>::empty())
		fill_buffer();

	return value;
}

template <typename T>
auto InputTape<T>::pop_back() -> T {
	T value = Tape<T>::pop_back();

	if (Tape<T>::empty())
		fill_buffer();

	return value;
}

template <typename T>
void InputTape<T>::open() {
	get_stream().open(get_path(), std::ios::in
	                            | std::ios::binary);
	get_stream().seekg(get_offset());
}
