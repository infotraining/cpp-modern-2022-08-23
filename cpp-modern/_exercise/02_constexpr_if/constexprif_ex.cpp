#include "catch.hpp"

#include <iostream>
#include <list>
#include <vector>

using namespace std;

struct Fast
{
};

struct Slow
{
};

namespace Cpp17
{
    template <typename Iterator>
    auto advance_it(Iterator& it, size_t n)
    {
        using ItCategory = typename std::iterator_traits<Iterator>::iterator_category;

        if constexpr (std::is_base_of_v<std::random_access_iterator_tag, ItCategory>)
        {
            it += n;
            return Fast {};
        }
        else
        {
            for (size_t i = 0; i < n; ++i)
                ++it;
            return Slow {};
        }
    }
}

namespace Cpp20
{
    template <std::input_iterator Iterator>
    auto advance_it(Iterator& it, size_t n)
    {        
        if constexpr (std::random_access_iterator<Iterator>)
        {
            it += n;
            return Fast {};
        }
        else
        {
            for (size_t i = 0; i < n; ++i)
                ++it;
            return Slow {};
        }
    }
}

TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator")
    {
        vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = Cpp20::advance_it(it, 3);

        static_assert(std::is_same_v<decltype(result), Fast>);

        REQUIRE(*it == 4);
    }

    SECTION("input_iterator")
    {
        list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = Cpp20::advance_it(it, 3);

        static_assert(std::is_same_v<decltype(result), Slow>);

        REQUIRE(*it == 4);
    }
}