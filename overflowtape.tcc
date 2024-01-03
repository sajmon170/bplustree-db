template <typename T>
auto OverflowTape<T>::pop_front() -> T {
	T value = Tape<T>::pop_front();

	if (Tape<T>::empty())
		Tape<T>::clear();

	return value;
}
