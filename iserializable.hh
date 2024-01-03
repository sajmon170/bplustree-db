#pragma once

#include <istream>
#include <ostream>

class ISerializable {
public:
	virtual void serialize(std::ostream&) const = 0;
	virtual void deserialize(std::istream&) = 0;
	virtual auto size_on_disk() const -> std::size_t = 0;
};
