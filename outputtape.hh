#pragma once

#include "tape.hh"

#include <algorithm>

template <typename T>
class OutputTape : public Tape<T> {
	using Tape<T>::get_buffer;
	using Tape<T>::get_size;
	using Tape<T>::get_offset;
	using Tape<T>::get_stream;
	using Tape<T>::get_path;

	bool logging_enabled = false;
	int write_count = 0;

public:
	OutputTape() = default;
	OutputTape(std::filesystem::path path,
	           std::uint64_t size,
	           std::size_t offset = 0)
		: Tape<T>::Tape(path, size, offset) {
		open();
	}

	void open();

	void save();
	void save(Tape<T>&);
	
	void push(T const& value);

	inline void extract_front(Tape<T>& other) {
		push(other.pop_front());
	}
	
	inline void extract_back(Tape<T>& other) {
		push(other.pop_back());
	}

	inline void enable_logging() {
		write_count = 0;
		logging_enabled = true;
	}

	inline void disable_logging() {
		logging_enabled = false;
	}

	inline auto get_stats() -> int {
		int result = write_count;
		write_count = 0;
		return result;
	}
};

#include "outputtape.tcc"
