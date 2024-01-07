#pragma once

#include "iserializable.hh"

class Point2 : public ISerializable {
	double x;
	double y;

public:
	Point2() = default;
	Point2(double x, double y)
		: x(x), y(y) {}
	
	void serialize(std::ostream&) const;
	void deserialize(std::istream&);
	auto size_on_disk() const -> std::size_t;
	
	inline auto operator==(Point2 const& other) const -> bool {
		return x == other.x && y == other.y;
	}
	
	auto constexpr getX() const& -> double { return x; }
	auto constexpr getY() const& -> double { return y; }
	friend auto operator<<(std::ostream&, Point2 const&) -> std::ostream&; 
};
