#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#define MSVC

#ifdef GCC_CLANG
#define PRINT_SIGNATURE __PRETTY_FUNCTION__
#else
#define PRINT_SIGNATURE __FUNCSIG__
#endif

using namespace std;

namespace Explain
{
    auto a1 = 10;

    template <typename T1, typename T2>
    void deduce_type(T1 arg1, T2 arg2)
    {
    }

    void deduce_type_cpp20(auto arg1, auto arg2)
    {
    }

    void use()
    {
        deduce_type_cpp20(10, 3.14);
    }
}

void foo(int)
{
}

template <typename T>
void deduce1(T arg)
{
    puts(PRINT_SIGNATURE);
}

template <typename T>
void deduce2(T& arg)
{
    puts(PRINT_SIGNATURE);
}

template <typename T>
void deduce3(T&& arg)
{
    puts(PRINT_SIGNATURE);
}

namespace Explain
{
    template <typename T>
    size_t size(const T& container)
    {
        return container.size();
    }

    template <typename T, size_t N>
    size_t size(T (&tab)[N])
    {
        return N;
    }
}

TEST_CASE("std::size - since C++17")
{
    std::vector vec = {1, 2, 3};
    REQUIRE(std::size(vec) == 3);

    int tab[10];
    REQUIRE(std::size(tab) == 10);
}

TEST_CASE("Type deduction rules")
{
    SECTION("Case1")
    {
        int x = 10;
        int& ref_x = x;
        const int& cref_x = x;
        const int cx = 10;
        int tab[10];

        deduce1(x);  //  deduce<int>(int)
        auto a1 = x; // int

        deduce1(ref_x);  // deduce<int>(int)
        auto a2 = ref_x; // int

        deduce1(cref_x);  // deduce<int>(int)
        auto a3 = cref_x; // const int

        deduce1(cx);  // deduce<int>(int)
        auto a4 = cx; // int

        deduce1(tab);  // deduce<int*>(int*) - decay an array to pointer
        auto a5 = tab; // int*
    }

    cout << "\n";

    SECTION("Case2")
    {
        int x = 10;
        int& ref_x = x;
        const int& cref_x = x;
        const int cx = 10;
        int tab[10];

        deduce2(x);   // deduce2<int>(int&)
        auto& a1 = x; // int&

        deduce2(ref_x);   // deduce2<int>(int&)
        auto& a2 = ref_x; // int&

        deduce2(cref_x);   // deduce2<const int>(const int&)
        auto& a3 = cref_x; // const int&

        deduce2(cx);   // deduce2<const int>(const int&)
        auto& a4 = cx; // const int&

        deduce2(tab);   // deduce2<int[10]>(int(&)[10])
        auto& a5 = tab; // int (&a5)[10]
    }

    cout << "\n";

    SECTION("Case3 - type deduction")
    {
        int x = 10;
        int& ref_x = x;
        const int& cref_x = x;
        int tab[10];

        deduce3(x);    // deduce3<int&>(int& && -> int&) -> deduce3<int&>(int&)
        auto&& a1 = x; // int&

        deduce3(42);    // deduce3<int>(int&&)
        auto&& a2 = 42; // int&&
    }
}

template <typename T>
decltype(auto) fwd(T&& item)
{
    if constexpr(std::is_reference_v<T>)
    {
        return item;
    }
    else
    {
        return std::move(item);
    }
}


template <typename T>
auto create_vec(T&& item)
{
    std::vector<std::remove_cvref_t<T>> items;

    // if constexpr(std::is_reference_v<T>)
    //     items.push_back(item);
    // else
    //     items.push_back(std::move(item)); // static_cast<T&&>(item)
    
    items.push_back(std::forward<T>(item));

    return items;
}

TEST_CASE("create_vec")
{
    using namespace std::literals;
    SECTION("passing rvalue")
    {
        auto vec = create_vec(std::string("abc"));
        REQUIRE(vec.front() == "abc"s);
    }

    SECTION("passing lvalue")
    {
        auto s = "abc"s;
        auto vec = create_vec(s); // passing s as lvalue -> create<string&>(string& item) -> forward<>
        REQUIRE(vec.front() == "abc"s);

        REQUIRE(s.size() == 3);
    }
}