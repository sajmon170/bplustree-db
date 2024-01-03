#pragma once

#include "tape.hh"

template <typename T>
class OverflowTape : public Tape<T> {
	using Tape<T>::get_buffer;

public:
	OverflowTape() = default;
	OverflowTape(std::uint64_t size)
		: Tape<T>::Tape("", size) {}

	inline void push(T const& value) {
		get_buffer().push_back(value);
	}

	inline void open() {
		return;
	}

	auto pop_front() -> T;
};

#include "overflowtape.tcc"
