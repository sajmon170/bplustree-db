#pragma once

#include "record.hh"
#include "point2.hh"

class Triangle : public Record {
	Point2 a;
	Point2 b;
	Point2 c;
	int key;

public:
	Triangle() = default;
	Triangle(Point2 const& a, Point2 const& b, Point2 const& c, int k)
		: a(a), b(b), c(c), key(k) {}

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

	inline auto getKey() const -> int {
		return key;
	}

	auto area() const -> double;
};
