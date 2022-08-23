#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <ranges>

#include "catch.hpp"

namespace BeforeCpp17
{
    std::tuple<int, int, double> calc_stats(const std::vector<int>& data)
    {
        std::vector<int>::const_iterator min_pos, max_pos;
        std::tie(min_pos, max_pos) = std::minmax_element(data.begin(), data.end());

        double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

        return std::make_tuple(*min_pos, *max_pos, avg);
    }
}

template <typename TCollection>
std::tuple<int, int, double> calc_stats(const TCollection& data)
{

    const auto [min_pos, max_pos] = std::minmax_element(std::ranges::begin(data), std::ranges::end(data));

    double avg = std::accumulate(std::ranges::begin(data), std::ranges::end(data), 0.0) / std::ranges::size(data); // since C++17 std::size()

    return { *min_pos, *max_pos, avg };
}

TEST_CASE("Before C++17")
{
    std::vector<int> data = {4, 42, 665, 1, 123, 13};

    auto result = calc_stats(data);

    REQUIRE(std::get<0>(result) == 1);

    int min, max;
    double avg;

    std::tie(min, max, avg) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

TEST_CASE("Since C++17")
{
    int data[] = { 4, 42, 665, 1, 123, 13 };

    auto [min, max, avg] = calc_stats(data); // structured bindings

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}