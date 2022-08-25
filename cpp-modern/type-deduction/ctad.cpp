#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <array>

#include "catch.hpp"

using namespace std::literals;

template <typename T1, typename T2>
struct ValuePair
{
	T1 fst;
	T2 snd;

	ValuePair(const T1& f, const T2& s) : fst{f}, snd{s}
	{}
};

// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

template <typename T>
ValuePair(T, const char*) -> ValuePair<T, std::string>;

template <typename T1, typename T2>
	requires (std::is_base_of_v<T1, T2> || std::is_base_of_v<T2, T1>)
ValuePair(T1* a, T2* b) -> ValuePair<std::common_type_t<T1*, T2*>, std::common_type_t<T1*, T2*>>;

struct X
{
};

struct Y : X
{
};

TEST_CASE("CTAD")
{
	ValuePair<int, double> v1{42, 3.14}; // C++98

	ValuePair v2{42, 3.14}; // ValuePair<int, double>

	const int x = 10;
	ValuePair v3{x, "abc"}; // ValuePair<int, std::string>

	ValuePair v4 = v3; // copy constructor

	ValuePair v5{std::vector{1, 2, 3}, "vec"s}; // ValuePair<std::vector<int>, std::string>

	std::vector vec = {1, 2, 3}; // std::vector<int>

	std::vector vec1{vec}; // std::vector<int>
	std::vector vec2{vec, vec}; // std::vector<std::vector<int>>

	ValuePair v6{new X(), new X()}; // 
}

namespace ExplainDiamondOp
{
	template <typename T = void>
	struct Greater
	{
		bool operator()(const T& a, const T& b) const
		{
			return a > b;
		}
	};

	template <>
	struct Greater<void>
	{
		template <typename T1, typename T2>
		bool operator()(const T1& a, const T2& b) const
		{
			return a > b;
		}
	};
}

// namespace std
// {
// 	template <std::integral T>
// 		requires (sizeof(T) >=4)
// 	vector(std::initializer_list<T>) -> vector<long>;
// }

TEST_CASE("diamond operator")
{
	std::vector vec = {5, 645, 234, 67, 3456, 876, 3453, 444};

	std::vector vec_chars = { 'a', 'b' };

	std::sort(vec.begin(), vec.end(), std::greater<int>{}); 
	std::sort(vec.begin(), vec.end(), std::greater{});
	std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) { return a > b; }); 

	std::string s1{"ABC"};

	REQUIRE(ExplainDiamondOp::Greater<void>{}(s1, "abc"));
	REQUIRE(std::is_sorted(vec.begin(), vec.end()));

	std::vector backup(vec.begin(), vec.end());
}

//////////////////////////////////
// CTAD aggrgates in C++17 & C++20

template <typename T1, typename T2>
struct Data
{
	T1 value;
	T2 name;
};

// template <typename T1, typename T2>
// Data(T1, T2) -> Data<T1, T2>;

TEST_CASE("aggregates")
{
	Data d1{42, "d1"};

	auto sptr = std::make_shared<Data<int, std::string>>(42, "d2");
}

///////////////////////////////////////////////////////////
// CTAD in std lib

TEST_CASE("pair")
{
	std::pair<int, std::string> p1{1, "text"};
	auto p2 = std::make_pair(42, "text"s);

	std::pair p3{665, "text"s};

	int tab[10] = {};

	std::pair p4{tab, "tab"}; // pair<int*, const char*>
}

TEST_CASE("tuple")
{
	std::tuple tpl1{1, 3.14, "text"}; // std::tuple<int, double, const char*>

	std::pair p{665, "text"s};
	std::tuple t2 = p;
}

TEST_CASE("optional")
{
	std::optional opt1{42}; // std::optional<int>
	
	std::optional opt2{opt1};   // std::optional<int>
	std::optional opt3 = opt1;  // std::optional<int>
}

TEST_CASE("smart pointers")
{
	std::unique_ptr<int> uptr1{new int(42)}; // CTAD disabled
	auto uptr2 = std::make_unique<std::vector<int>>(10, 1);

	std::shared_ptr<std::vector<int>> sptr1{new std::vector<int>(10, -1)}; // CTAD disabled

	std::shared_ptr sptr2 = std::move(uptr1);
	std::weak_ptr wptr2 = sptr2;
}

TEST_CASE("function")
{
	std::function<int(int, int)> f1 = [](int a, int b) { return a + b; };

	std::function f2 = [](int a, int b) { return a + b; }; // CTAD
}

TEST_CASE("containers")
{
	std::vector vec = {1, 2, 3, 4}; // std::vector<int>
	REQUIRE(vec == std::vector{1, 2, 3, 4});

	std::list lst(vec.begin(), vec.end()); // std::list<int>
}

TEST_CASE("array")
{
	std::array arr = {1, 2, 3, 4, 5, 6};
}