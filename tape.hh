#pragma once

#include <vector>
#include <cstdint>
#include <fstream>
#include <filesystem>

//for debugging
#include <iostream>

template <typename T>
class Tape {
	std::fstream file;
	std::filesystem::path path;
	std::vector<T> buffer;
	std::uint64_t size;
	std::size_t offset;

	std::size_t pos_beg = 0;

protected:
	inline auto get_buffer() -> std::vector<T>& {
		return buffer;
	}

	inline auto get_stream() -> std::fstream& {
		return file;
	}

	inline auto get_path() const -> std::filesystem::path {
		return path;
	}

	inline void set_path(std::filesystem::path new_path) {
		path = new_path;
	}

	inline void advance_beg() {
		++pos_beg;
	}
	
public:
	Tape() = default;
	Tape(std::filesystem::path, std::uint64_t, std::size_t offset = 0);

	void set_file(std::filesystem::path new_path, std::size_t offset = 0);

	void _debug_areas() {
		for (auto& element: buffer)
			std::cout << element.area() << " ";
		std::cout << "\n";
	}

	inline auto get_size() const -> std::uint64_t {
		return size;
	}

	inline auto get_offset() const -> std::size_t {
		return offset;
	}

	inline auto set_offset(std::size_t value) {
		offset = value;
	}

	inline auto begin() const -> typename std::vector<T>::const_iterator {
		return buffer.begin() + pos_beg;
	}

	inline auto begin() -> typename std::vector<T>::iterator {
		return buffer.begin() + pos_beg;
	}
	
	inline auto end() const -> typename std::vector<T>::const_iterator {
		return buffer.end();
	}

	inline auto end() -> typename std::vector<T>::iterator {
		return buffer.end();
	}

	inline auto front() -> T& {
		return buffer[pos_beg];
	}

	inline auto back() -> T& {
		return buffer.back();
	}

	inline auto empty() const -> bool {
		return buffer.size() == pos_beg;
	}

	inline auto full() const -> bool {
		return buffer.size() == size;
	}

	inline void close() {
		buffer.clear();
		file.close();
	}

	inline auto count() const -> std::size_t {
		return buffer.size();
	}

	inline void clear() {
		buffer.clear();
		pos_beg = 0;
	}
	
	inline auto good() const {
		return file.good() || !empty();
	}

	inline auto operator[](std::size_t idx) -> T& {
		return buffer[idx];
	}

	inline auto operator[](std::size_t idx) const -> T {
		return buffer[idx];
	}
	
	inline operator bool() const {
		return good();
	}

	inline void swap_with(Tape<T>& other) {
		buffer.swap(other.get_buffer());
	}
	
	virtual void push(T const&);
	virtual auto pop_front() -> T;
	virtual auto pop_back() -> T;
	virtual void open() = 0;
};

#include "tape.tcc"
