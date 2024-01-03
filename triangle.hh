#pragma once

#include "record.hh"
#include "point2.hh"

class Triangle : public Record {
	Point2 a;
	Point2 b;
	Point2 c;

public:
	Triangle() = default;
	Triangle(Point2 const& a, Point2 const& b, Point2 const& c)
		: a(a), b(b), c(c) {}

	auto operator<(Record const&) const -> bool;
	auto operator==(Record const&) const -> bool;
	void print(std::ostream&) const;
	void serialize(std::ostream&) const;
	void deserialize(std::istream&);
	auto size_on_disk() const -> std::size_t;
	
	inline auto getA() const -> Point2 {
		return a;
	}

	inline auto getB() const -> Point2 {
		return b;
	}

	inline auto getC() const -> Point2 {
		return c;
	}

	auto area() const -> double;
};
