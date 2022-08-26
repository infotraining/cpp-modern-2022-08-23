#include "catch.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using namespace std::literals;

namespace Unsafe
{
    double calculate_speed(double distance, double time)
    {
        return distance / time;
    }
}

template <typename R = std::ratio<1>>
class distance
{
	long double value_{};
public:
	constexpr distance() = default;
	constexpr distance(long double value) : value_{value}
	{}

	constexpr double si_value()
	{
		return value_ * (R::num / static_cast<double>(R::den));
	}
};

class velocity
{
    double value_ {};

public:
    constexpr velocity() = default;

    constexpr velocity(double v)
        : value_ {v}
    { }

    constexpr double si_value() const
    {
        return value_;
    }
};

using meters = distance<>;
using kilometers = distance<std::kilo>;

constexpr meters operator"" _m(long double value)
{
	return meters{value};
} 

constexpr kilometers operator"" _km(long double value)
{
	return kilometers{value};
}

template <typename R, typename Rep, typename Period>
constexpr auto calculate_speed(distance<R> ds, std::chrono::duration<Rep, Period> dt)
{
	return velocity {ds.si_value() / std::chrono::duration_cast<std::chrono::seconds>(dt).count()};
}

TEST_CASE("user literals")
{
    constexpr auto speed_1 = calculate_speed(100.0_km, 10h);
	constexpr auto speed_2 = calculate_speed(3.0_m, 1s);
}