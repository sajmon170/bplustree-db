#include "record.hh"

auto operator<<(std::ostream& out, Record const& record) -> std::ostream& {
	record.print(out);
	return out;
}

auto operator>>(std::istream& in, Record& record) -> std::istream& {
	record.deserialize(in);
	return in;
}

