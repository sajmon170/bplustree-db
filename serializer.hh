#pragma once
#include <ostream>
#include <istream>

// TODO: add an std::vector serializer/deserializer specialization

namespace Serializer {
	template <typename T>
	inline void serialize(std::ostream& out, T const& value) {
		out.write(reinterpret_cast<char const*>(&value), sizeof(T));
	}

	template <typename T>
	inline void deserialize(std::istream& in, T& value) {
		in.read(reinterpret_cast<char*>(&value), sizeof(T));
	}

	void pad_out(std::ostream&, std::size_t const&, char const value = 0);

	template <typename T>
	inline void allocate(std::ostream& out, std::size_t count = 1) {
		pad_out(out, count * sizeof(T));
	}
}
