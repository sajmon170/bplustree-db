#include "triangle.hh"

#define abs(x) ((x) > 0 ? (x) : -(x))

auto Triangle::operator<(Record const& record) const -> bool {
	auto& other = static_cast<Triangle const&>(record);
	return this->area() < other.area();
}

auto Triangle::operator==(Record const& record) const -> bool {
	auto& other = static_cast<Triangle const&>(record);
	return a == other.a && b == other.b && c == other.c;
}

void Triangle::print(std::ostream& out) const {
	out << "A: "      << getA()
		<< ", B: "    << getB()
		<< ", C: "    << getC()
		<< ", Area: " << area();
}

void Triangle::serialize(std::ostream& file) const {
	a.serialize(file);
	b.serialize(file);
	c.serialize(file);
}

void Triangle::deserialize(std::istream& file) {
	a.deserialize(file);
	b.deserialize(file);
	c.deserialize(file);
}

auto Triangle::size_on_disk() const -> std::size_t {
	return a.size_on_disk() + b.size_on_disk() + c.size_on_disk();
}
	

inline auto Triangle::area() const -> double {
	return 0.5 * abs(a.getX()*(b.getY() - c.getY())
	               + b.getX()*(c.getY() - a.getY())
	               + c.getX()*(a.getY() - b.getY()));
}
