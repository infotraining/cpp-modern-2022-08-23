#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <array>
#include <cmath>

#include "catch.hpp"

int runtime_func(int x)
{
    return x * x;
}
 
constexpr int constexpr_func(int x) 
{
    return x * x;
}
 
consteval int consteval_func(int x)
{
    return x * x;
}

TEST_CASE("constexpr")
{
	int s1 = runtime_func(42);

	static_assert(constexpr_func(8) == 64);
	
	s1 = constexpr_func(10);

	constexpr std::array<int, consteval_func(8)> data = { consteval_func(1), constexpr_func(2), consteval_func(3) };

	s1 = consteval_func(13);
}

constexpr bool is_prime(int n)
{
    int limit;
    if (std::is_constant_evaluated())
        limit = n / 2;
    else
    {
        limit = std::sqrt(n);
        std::cout << "runtime check for "
                  << n << "\n";
    }

    for (int i = 2; i <= limit; ++i)
        if (n % i == 0)
            return false;
    return n > 1;
}

template <int N>
consteval std::array<int, N> prime_numbers()
{
    std::array<int, N> primes {};

    int idx = 0;
    for (int n = 1; idx < N; ++n)
    {
        if (is_prime(n))
        {
            primes[idx++] = n;
        }
    }
    return primes;
}

template <std::ranges::input_range Rng>
constexpr auto median(const Rng& rng)
{
    if (std::ranges::empty(rng))
        return 0.0;

    std::vector data(std::ranges::begin(rng), std::ranges::end(rng));

    std::ranges::sort(data);
    
    size_t size = std::ranges::size(data);
    size_t middle = size / 2;
    
    if (size % 2 == 0)
    {        
        return (data[middle - 1] + data[middle]) / 2.0;
    }

    return static_cast<double>(data[middle]);
}

template <std::ranges::range TCollection> 
void print(TCollection&& coll, std::string_view desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : coll)
		std::cout << item << " ";
	std::cout << "]\n";
}

TEST_CASE("is_prime")
{
	static_assert(is_prime(13));

	REQUIRE(is_prime(13));

	constexpr std::array primes = prime_numbers<100>();

	std::array primes_runtime = prime_numbers<200>();

	print(primes_runtime, "primes_runtime");

	constexpr std::array data{3, 2, 1};
	static_assert(median(data) == 2);
}