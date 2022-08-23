#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

TEST_CASE("if with initializer")
{
    std::vector vec = {1, 2, 3, 42, 66, 34};

    if (auto it = std::find(vec.begin(), vec.end(), 42);
        it != vec.end())
    {
        std::cout << "Found: " << *it << "\n";
    }
    else
    {
        assert(it == vec.end());
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    SECTION("thread#1")
    {
        std::lock_guard lk {mtx_q_msg}; // CTAD
        q_msg.push("START");
    }

    SECTION("thread#2")
    {
        std::string msg;

        if (std::lock_guard lk {mtx_q_msg}; !std::empty(q_msg))
        {
            msg = q_msg.front();
            q_msg.pop();
        } // automatic unlock
    }
}

///////////////////////////////////////////////
// constexpr if

// template <typename T>
// bool is_power_of_2(T value)
// {
//     return value > 0 && (value & (value - 1)) == 0;
// }

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral<T>::value, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point<T>::value, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

template <typename T>
auto is_power_of_2(T value)
{
    if constexpr (std::is_integral<T>::value)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }
    else
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

TEST_CASE("constexpr if")
{
    REQUIRE(is_power_of_2(8));
    REQUIRE(is_power_of_2(64));
    REQUIRE_FALSE(is_power_of_2(10));

    REQUIRE(is_power_of_2(8.0));
}

namespace BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename THead, typename... TTail>
    void print(const THead& head, const TTail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

template <typename THead, typename... TTail>
void print(const THead& head, const TTail&... tail)
{
    std::cout << head << " ";
    
    if constexpr (sizeof...(tail) > 0)
    {        
        print(tail...);
    }
    else
    {
        std::cout << "\n";
    }
}

TEST_CASE("variadic templates")
{
    print(1, 3.14, "abc", std::string("def"));
}
