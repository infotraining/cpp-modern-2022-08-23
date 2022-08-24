#include "catch.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

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

    return {*min_pos, *max_pos, avg};
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
    int data[] = {4, 42, 665, 1, 123, 13};

    auto [min, max, avg] = calc_stats(data); // structured bindings

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

auto get_coordinates() -> int (&)[2]
{
    static int coord[] = {1, 2};

    return coord;
}

std::array<int, 3> get_coord3D()
{
    return {1, 2, 3};
}

struct ErrorCode
{
    int ec;
    const char* m;
};

ErrorCode open_file()
{
    return {13, "file not found"};
}

TEST_CASE("structured bindings")
{
    SECTION("native array")
    {
        auto [x, y] = get_coordinates();

        REQUIRE(x == 1);
        REQUIRE(y == 2);
    }

    SECTION("std array")
    {
        auto [x, y, z] = get_coord3D();
    }

    SECTION("std::pair")
    {
        std::set<int> values;

        auto [pos, was_inserted] = values.insert(42);

        REQUIRE(was_inserted);
    }

    SECTION("struct/class")
    {
        auto [error_code, error_message] = open_file();

        REQUIRE(error_code == 13);
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("how it works")
{
    auto [hours, minutes, seconds] = Timestamp {1, 20, 45};

    SECTION("is interpreted as")
    {
        auto entity = Timestamp {1, 20, 45};
        auto& hours = entity.h;
        auto& minutes = entity.m;
        auto& seconds = entity.s;
    }
}

TEST_CASE("use cases")
{
    SECTION("iteration over map")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "one"}, {3, "three"}};

        SECTION("Before C++17")
        {
            for (const auto& kv : dict)
            {
                std::cout << kv.first << " - " << kv.second << "\n";
            }
        }

        SECTION("Since C++17")
        {
            for (const auto& [key, value] : dict)
            {
                std::cout << key << " - " << value << "\n";
            }
        }
    }

    SECTION("for + multiple declaraion")
    {
        std::list lst {"one", "two", "three"};

        for (auto [index, it] = std::tuple {0, std::ranges::begin(lst)}; it != std::ranges::end(lst); ++index, ++it)
        {
            std::cout << index << " - " << *it << "\n";
        }
    }
}

//////////////////////////////////////////////////////
// tuple-like protocol

class Person
{
    std::string fname_;
    std::string lname_;
public:
    Person(std::string fn, std::string ln) : fname_{fn}, lname_{ln}
    {}

    const std::string& first_name() const
    {
        return fname_;
    }

    const std::string& last_name() const
    {
        return fname_;
    }

    std::string& first_name()
    {
        return fname_;
    }

    std::string& last_name()
    {
        return fname_;
    }
};

template <>
struct std::tuple_size<Person>
{
    static constexpr size_t value = 2;
};

template <std::size_t Index>
struct std::tuple_element<Index, Person>
{
    using type = std::string;
};

template <size_t Index, typename T>
decltype(auto) get(T&& p)
{
    if constexpr(Index == 0)
        return p.first_name();
    else 
        return p.last_name();
}

TEST_CASE("Person + structured binding")
{
    Person p1{"Jan", "Kowalski"};

    SECTION("auto by value")
    {
        auto [name, surname] = p1;
        name = "Adam";

        REQUIRE(p1.first_name() == "Jan");
    }

    SECTION("auto by &")
    {
        auto& [name, surname] = p1;
        name = "Adam";

        REQUIRE(p1.first_name() == "Adam");
    }

    SECTION("auto by &&")
    {
        auto&& [name, surname] = p1;
        name = "Adam";

        REQUIRE(p1.first_name() == "Adam");
    }
}