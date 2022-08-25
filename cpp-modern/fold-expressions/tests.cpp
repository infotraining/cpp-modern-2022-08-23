#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "catch.hpp"

namespace VariadicTemplates
{
	template <typename... Ts>
	struct Tuple
	{};

	template <typename T, typename... TArgs>
	std::unique_ptr<T> make_unique(TArgs&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
	}
}

TEST_CASE("using variadic templates")
{
	using namespace VariadicTemplates;
	
	Tuple<int, double, std::string> t1;
	Tuple<> t2;

	auto ptr = VariadicTemplates::make_unique<std::pair<int, std::string>>(42, std::string("forty_two"));
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

// sum(head = 1, tail... = (2, 3, 4, 5))  :  
//   -> sum(head = 2, tail... = (3, 4, 5)):  
//     -> sum(head = 3, tail... = (4, 5)) :  
//	     -> sum(head = 4, tail... = (5))  :  

template <typename... TArgs>
auto sum(const TArgs&... args) // sum(1, 2, 3, 4, 5)
{
	return (... + args); // left unary fold
}

template <typename... TArgs>
void print(const TArgs&... args)
{
	auto with_space = [is_first = true](const auto& item) mutable
    {
        if (!is_first)
            std::cout << " ";
        is_first = false;
        return item;
    };

	(std::cout << ... << with_space(args)) << "\n";
}

template <typename... TArgs>
void print_lines(const TArgs&... args)
{
	(..., (std::cout << args << "\n"));
}

template <typename F, typename... TArgs>
void call_for_all(F f, const TArgs&... args)
{
	(..., f(args));
}



TEST_CASE("fold expressions")
{
	REQUIRE(sum(1, 2, 3, 4, 5) == 15);

	print(1, 3.14, "test");
	print_lines(1, 3.14, "test");

	auto foo = [](auto value)
	{
		std::cout << "foo(" << value << ")\n";
	};

	call_for_all(foo, 1, 3.14, "test");
}