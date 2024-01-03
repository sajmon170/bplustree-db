#pragma once

#include "tape.hh"

#include <filesystem>
#include <algorithm>

#include <iostream>

template <typename T>
class OutputTape;

template <typename T>
class InputTape : public Tape<T> {
	using Tape<T>::get_buffer;
	using Tape<T>::get_size;
	using Tape<T>::get_offset;
	using Tape<T>::get_stream;
	using Tape<T>::get_path;

	std::size_t record_size;

	bool is_limited = false;
	bool refill_lock = false;
	std::size_t refill_limit;

	bool logging_enabled = false;
	int read_count = 0;

protected:
	inline void lock() {
		refill_lock = true;
	}

	inline void unlock() {
		refill_lock = false;
	}
	
public:
	InputTape() = default;
	InputTape(std::filesystem::path path,
	          std::uint64_t size,
	          std::size_t offset = 0)
		: Tape<T>::Tape(path, size, offset) {

		open();
		T record;
		record_size = record.size_on_disk();
	}

	void open();

	auto fill_buffer() -> bool;

	inline void set_refill_limit(std::size_t limit) {
		is_limited = true;
		refill_limit = limit;
	}

	inline void unset_refill_limit() {
		is_limited = false;
		unlock();
	}

	inline void enable_logging() {
		read_count = 0;
		logging_enabled = true;
	}

	inline void disable_logging() {
		logging_enabled = false;
	}

	inline auto get_stats() -> int {
		int result = read_count;
		read_count = 0;
		return result;
	}
	
	inline void seek(std::size_t position) {
		get_stream().clear();
		get_stream().seekg(get_offset() + position);
	}
	
	inline void view_tape(int tape_no) {
		seek(tape_no * get_size() * record_size);
	}
	
	inline void shift_view(int tape_count = 1) {
		get_stream().ignore(tape_count * get_size() * record_size);
	}

	inline auto is_locked() -> bool {
		return refill_lock;
	}

	inline auto is_valid() -> bool {
		return get_stream().good() && !is_locked();
	}

	auto pop_front() -> T override;
	auto pop_back() -> T override;
	
	inline auto pop_back_without_refill() -> T {
		return Tape<T>::pop_back();
	}

	inline auto pop_front_without_refill() -> T {
		return Tape<T>::pop_front();
	}

	inline auto operator>>(T& value) -> Tape<T>& {
		value = pop_back();
		return *this;
	}
	
	friend class OutputTape<T>;
};

#include "inputtape.tcc"
