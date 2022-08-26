#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <ranges>
#include <string_view>

#include "catch.hpp"

template <std::ranges::range TCollection> 
void print(TCollection&& coll, std::string_view desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : coll)
		std::cout << item << " ";
	std::cout << "]\n";
}

// template <std::ranges::view TView> 
// void print(TView coll, std::string_view desc)
// {
// 	std::cout << desc << ": [ ";
// 	for(const auto& item : coll)
// 		std::cout << item << " ";
// 	std::cout << "]\n";
// }

template <auto Value_>
struct EndValue
{
    bool operator==(auto pos) const
    {
        return *pos == Value_;
    }
};

TEST_CASE("ranges")
{
	std::vector vec = {1, 4, 6, 34, 567, 87, 23, 343, 665, 42, 12, 7, 5 };

	std::ranges::sort(vec);

	print(vec, "vec");

	SECTION("projection")
	{
		std::vector<std::string> words{"c", "c++", "c++20", "template"};

		std::ranges::sort(words, [](const auto& a, const auto& b) { return a.size() < b.size() ;});
		print(words, "words");

		std::ranges::sort(words, std::greater{}, [](const auto& w) { return w.size(); });
		print(words, "words");
	}

	SECTION("sentinels")
	{
		std::ranges::sort(vec.begin(), EndValue<87>{}, std::greater{});

		print(vec, "vec sorted with sentinel");

		auto slice = std::ranges::subrange(vec.begin(), EndValue<87>{});

		print(slice, "slice");
	}

	SECTION("views")
	{
		auto all = std::views::all(vec);
		print(all, "all items");

		auto head = vec | std::views::take(5);
		print(head, "head");

		print(vec | std::views::drop(5), "tail");

		auto evens = vec 
			| std::views::filter([](int n) { return n % 2 == 0;}) 
			| std::views::take(3) 
			| std::views::reverse;

		print(evens, "evens");

		std::vector vec_evens(evens.begin(), evens.end());

		print(vec_evens, "vec_evens");
	}
}