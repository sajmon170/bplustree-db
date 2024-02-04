#pragma once

#include "serializer.hh"

#include <istream>
#include <ostream>

class ISerializable {
public:
	virtual void serialize(std::ostream&) const = 0;
	virtual void deserialize(std::istream&) = 0;
	virtual auto size_on_disk() const -> std::size_t = 0;
	virtual ~ISerializable() = default;
};

template <>
inline void Serializer::serialize<ISerializable>(std::ostream& out,
                                                 ISerializable const& value) {
	value.serialize(out);
}

template <>
inline void Serializer::deserialize<ISerializable>(std::istream& in,
                                                   ISerializable& value) {
	value.deserialize(in);
}
