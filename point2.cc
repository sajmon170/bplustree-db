#include "point2.hh"

auto operator<<(std::ostream& out, Point2 const& pt) -> std::ostream& {
	out << "{" << pt.getX() << ", " << pt.getY() << "}";
	return out;
}

void Point2::serialize(std::ostream& out) const {
	Serializer::serialize(out, x);
	Serializer::serialize(out, y);
}
	
void Point2::deserialize(std::istream& in) {
	Serializer::deserialize(in, x);
	Serializer::deserialize(in, y);
}

auto Point2::size_on_disk() const -> std::size_t {
	return sizeof(x) + sizeof(y);
}
