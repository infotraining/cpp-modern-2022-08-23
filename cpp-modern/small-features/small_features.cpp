#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <span>

#include "catch.hpp"

enum class DayOfWeek { mon = 1, tue, wed, thd, fri, sat, sun };

TEST_CASE("enum init")
{
	DayOfWeek today = DayOfWeek::tue;
	int value = static_cast<int>(today);

	SECTION("since C++17")
	{
		DayOfWeek tomorrow{3};
	}
}

TEST_CASE("byte mess")
{
	char bits_8 = 128;

	++bits_8;
	bits_8 += 15;

	char another_8_bits = 67;

	std::cout << (bits_8 + another_8_bits) << "\n";
}

namespace Explain
{
	enum class byte : uint8_t {};
}

TEST_CASE("std::byte")
{
	std::byte bits_8{128};
	std::byte another_8_bits{67};

	std::cout << std::to_integer<int>(bits_8 | another_8_bits) << std::endl;
}

void print(float const x, std::span<const std::byte> const bytes)
{
    std::cout << std::setprecision(6) << std::setw(8) << x << " = { "
              << std::hex << std::uppercase << std::setfill('0');
    for (auto const b : bytes) {
        std::cout << std::setw(2) << std::to_integer<int>(b) << ' ';
    }
    std::cout << std::dec << "}\n";
}
 
TEST_CASE("span of bytes")
{
    /* mutable */ float data[1] { 3.141592f };
 
    auto const const_bytes = std::as_bytes(std::span{ data });
 
    print(data[0], const_bytes);
 
    auto const writable_bytes = std::as_writable_bytes(std::span{ data });
 
    // Change the sign bit that is the MSB (IEEE 754 Floating-Point Standard).
    writable_bytes[3] |= std::byte{ 0b1000'0000 };
 
    print(data[0], const_bytes);
}

void print(std::span<const int> items)
{
	for(const auto& item : items)
		std::cout << item << " ";
	std::cout << "\n";
}

void may_throw()
{
	throw 42;
}

void foo() noexcept
{
	try
	{
		may_throw();
	}
	catch(...)
	{}
}

void bar()
{}

TEST_CASE("span")
{
	int tab1[] = {1, 2, 3, 4, 5};
	std::vector vec = {1, 2, 3, 4, 5};

	print(std::span{tab1});
	print(std::span{vec.begin(), 3});

	foo();

	void (*func_ptr)() noexcept{};
	// func_ptr = bar;
}

template <typename T>
void  deduce(T item)
{}

TEST_CASE("auto & type deduction in templates")
{
	auto a1 = "text";
	deduce("text");

	const int x = 10;
	

	auto& a2 = x;

	auto&& ax = 42;
}