#include "triangle.hh"

#define abs(x) ((x) > 0 ? (x) : -(x))

auto Triangle::operator<(Record const& record) const -> bool {
	auto& other = static_cast<Triangle const&>(record);
	return this->getKey() < other.getKey();
}

auto Triangle::operator==(Record const& record) const -> bool {
	auto& other = static_cast<Triangle const&>(record);
	return a == other.a && b == other.b && c == other.c;
}

void Triangle::print(std::ostream& out) const {
	out << "key: "    << getKey()
	    << ", A: "    << getA()
	    << ", B: "    << getB()
	    << ", C: "    << getC()
	    << ", Area: " << area();
}

void Triangle::serialize(std::ostream& file) const {
	Serializer::serialize(file, key);
	Serializer::serialize(file, a);
	Serializer::serialize(file, b);
	Serializer::serialize(file, c);
}

void Triangle::deserialize(std::istream& file) {
	Serializer::deserialize(file, key);
	Serializer::deserialize(file, a);
	Serializer::deserialize(file, b);
	Serializer::deserialize(file, c);
}

auto Triangle::size_on_disk() const -> std::size_t {
	return a.size_on_disk() + b.size_on_disk() + c.size_on_disk();
}
	

inline auto Triangle::area() const -> double {
	return 0.5 * abs(a.getX()*(b.getY() - c.getY())
	               + b.getX()*(c.getY() - a.getY())
	               + c.getX()*(a.getY() - b.getY()));
}
