#pragma once

#include "iserializable.hh"
#include <ostream>

class Record : public ISerializable {
public:
	virtual auto operator<(Record const&) const -> bool = 0;
	virtual auto operator==(Record const&) const -> bool = 0;
	virtual void print(std::ostream&) const = 0;

	auto inline operator<=(Record const& other) const -> bool {
		return (*this < other) || (*this == other);
	}

	auto inline operator>(Record const& other) const -> bool {
		return (other < *this);
	}

	auto inline operator>=(Record const& other) const -> bool {
		return (other <= *this);
	}
	
	friend auto operator<<(std::ostream&, Record const&) -> std::ostream&;
	friend auto operator>>(std::istream&, Record&) -> std::istream&;
};

