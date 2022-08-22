#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "catch.hpp"

std::tuple<int, int, double> calc_stats(const std::vector<int>& data)
{
    std::vector<int>::const_iterator min, max;
    std::tie(min, max) = std::minmax_element(data.begin(), data.end());

    double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

    return std::make_tuple(*min, *max, avg);
}

TEST_CASE("Before C++17")
{
    std::vector<int> data = {4, 42, 665, 1, 123, 13};

    int min, max;
    double avg;

    std::tie(min, max, avg) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}