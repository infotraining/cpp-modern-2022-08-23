#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <set>

namespace Explain
{
    ////////////////////////////////////////
    // IsIntegral
    template <typename T>
    struct IsIntegral : std::false_type
    {
    };
    // {
    //     constexpr static bool value = false;
    // };

    template <>
    struct IsIntegral<int> : std::true_type
    {
    };
    // {
    // 	constexpr static bool value = true;
    // };

    template <>
    struct IsIntegral<long> : std::true_type
    {
    };

    template <typename T>
    constexpr bool IsIntegral_v = IsIntegral<T>::value;

    /////////////////////////////////
    // IsPointer

    template <typename T>
    struct IsPointer : std::false_type
    {
    };

    template <typename T>
    struct IsPointer<T*> : std::true_type
    {
    };

    /////////////////////////////////
    // enable_if

    template <bool Condition, typename T = void>
    struct EnableIf
    {
        using type = T;
    };

    template <typename T>
    struct EnableIf<false, T>
    {
    };

    template <bool Condition, typename T = void>
    using EnableIf_t = typename EnableIf<Condition, T>::type;
} // namespace Explain

// template <typename T, auto Size, typename = void> // C++17
// struct Array;

template <typename T, auto Size> // C++17
struct Array
{
    static_assert(Explain::IsIntegral_v<T>);

    T items[Size];
};

template <double Value> // C++20
struct Vat
{
    double calculate_tax(double price)
    {
        return price * Value;
    }
};

TEST_CASE("NTTP")
{
    Array<int, 2032u> arr1;

    Vat<0.23> vat_pl;

    REQUIRE(vat_pl.calculate_tax(100.0) == 23.0);
}

///////////////////////////////////////////////////
// type traits

template <typename T>
struct TypeSize
{
    static constexpr size_t value = sizeof(T);
};

template <typename T>
constexpr auto TypeSize_v = TypeSize<T>::value;

template <typename T>
struct Identity
{
    using type = T;
};

template <typename T>
using Identity_t = typename Identity<T>::type;

// works OK for sizeof <= 8
template <typename T, typename = Explain::EnableIf_t<(sizeof(T) <= 8)>>
void do_stuff(T t)
{
    std::cout << "do stuff for small objects\n";
}

// works OK for sizeof > 8
template <typename T, typename = void, typename = Explain::EnableIf_t<(sizeof(T) > 8)>>
void do_stuff(const T& t)
{
    std::cout << "do stuff for large objects\n";
}

TEST_CASE("metafunction")
{
    static_assert(TypeSize_v<int> == 4);
    static_assert(std::is_same_v<int, Identity_t<int>>);

    static_assert(std::integral_constant<int, 5>::value == 5);
    static_assert(std::true_type::value);
    static_assert(!std::false_type::value);

    static_assert(std::is_same_v<int, std::remove_cv_t<const int>>);

    int tab[10];
    static_assert(std::is_same_v<int*, std::decay_t<decltype(tab)>>);

    do_stuff(42);
    do_stuff(std::string("abc"));
}

//////////////////////////////////
// is_power_2

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral_v<T>, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point_v<T>, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

namespace SinceCpp17
{
    template <typename T>
    auto is_power_of_2(T value)
    {
        if constexpr (std::is_integral_v<T>)
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
}

namespace SinceCpp20
{
    auto is_power_of_2(std::integral auto value)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <std::floating_point T>
    auto is_power_of_2(T value)
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

////////////////////////////////////////////////////
// void_t

namespace BeforeCpp20
{

    template <typename, typename = void>
    constexpr bool is_iterable_v {};

    template <typename T>
    constexpr bool is_iterable_v<
        T,
        std::void_t<
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end()),
            decltype(std::declval<T>().size())>> = true;
}

template <typename T>
concept Iterable = requires(T obj) {
	std::ranges::begin(obj);
	std::ranges::end(obj);
};

template <typename T>
concept Sizeable = Iterable<T> && requires (T obj) {
	std::ranges::size(obj);
};

void foo(Iterable auto coll)
{
	std::cout << "foo(Iterable auto coll)\n";
}

void foo(Sizeable auto coll)
{
	std::cout << "foo(Sizeable auto coll)\n";
}

TEST_CASE("void_t")
{
    static_assert(BeforeCpp20::is_iterable_v<int> == false);

    static_assert(Iterable<std::vector<int>>);

	foo(std::vector{1, 2, 3});
}

template <typename TCollection, typename TElement>
void add(TCollection& coll, TElement&& item)
{
	if constexpr(requires { coll.push_back(item); })
	{
		coll.push_back(std::forward<TElement>(item));
	}
	else
	{
		coll.insert(std::forward<TElement>(item));
	}
}

TEST_CASE("add")
{
	std::vector<int> vec;
	add(vec, 1);

	std::set<int> st;
	add(st, 1);
}