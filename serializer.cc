#include "serializer.hh"

namespace Serializer {
	void pad_out(std::ostream& out, std::size_t const& bytes, char const value) {
		for (std::size_t i = 0; i < bytes; ++i) {
			out.write(&value, sizeof(char));
			Serializer::bytes += sizeof(char);
		}
	}
}
