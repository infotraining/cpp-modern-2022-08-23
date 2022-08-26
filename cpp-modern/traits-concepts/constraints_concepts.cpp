#include "catch.hpp"

#include <algorithm>
#include <concepts>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

// template <typename T>
// concept IsPointer = std::is_pointer_v<T>;

template <typename T>
concept IsPointer = requires(T ptr)
{
    *ptr;
    ptr == nullptr;
    { ptr < ptr } -> std::convertible_to<bool>;
};

template <typename T>
    requires(!IsPointer<T>)
T max_value(T a, T b)
{
    return a < b ? b : a;
}

namespace ver_1
{
    template <typename T>
        requires IsPointer<T>
    auto max_value(T a, T b)
    {
        return max_value(*a, *b);
    }
}

namespace ver_2
{
    template <IsPointer T>
    auto max_value(T a, T b)
    {
        return max_value(*a, *b);
    }
}

auto max_value(IsPointer auto a, IsPointer auto b) requires std::three_way_comparable_with<decltype(*a), decltype(*b)>
{
    return max_value(*a, *b);
}

namespace IsInterpreted
{
    template <IsPointer T1, IsPointer T2>
    auto max_value(T1 a, T2 b)
    {
        return max_value(*a, *b);
    }
}

TEST_CASE("constraints")
{
    int x = 10;
    int y = 20;
    REQUIRE(max_value(x, y) == 20);

    REQUIRE(max_value(&x, &y) == 20);

    std::vector<int> vec = {1, 2, 3};
    std::random_access_iterator auto pos = vec.begin();
}

template <typename T>
concept PrintableRange = std::ranges::range<T> 
    && requires (std::ranges::range_value_t<T> obj) {
    std::cout << obj;
};

void print(const PrintableRange auto& rng)
{
    for(const auto& item : rng)
    {
        std::cout << item << " ";
    }
    std::cout << "\n";
}


template <typename T>
struct Value
{
    T value;

    void print() const        
    {
        std::cout << value << "\n";
    }

    void print() const 
        requires std::ranges::range<T>
    {
        ::print(value);
    }
};

TEST_CASE("printing ranges")
{
    std::vector vec = {1, 2, 3};
    print(vec);

    std::map<int, std::string> dict = { {1, "one"} };
    //print(dict); // ERROR - constained not satisfied

    Value v1{42};
    v1.print();

    Value v2{std::vector{1, 2, 3}};
    v2.print();
}

template <typename T>
    requires requires { typename T::value_type; }
void foo()
{
   typename  T::value_type temp{};   
}

template <typename T>
concept BeCareful = requires 
{
    requires std::is_integral_v<T>;
};

TEST_CASE("requires + requires")
{
    static_assert(BeCareful<int>);
}