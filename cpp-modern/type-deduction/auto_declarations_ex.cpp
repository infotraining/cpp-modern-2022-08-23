#include <iostream>
#include <type_traits>
#include <vector>
#include <map>

#include "catch.hpp"

using namespace std;

using TODO = void;

TEST_CASE("exercise")
{
    int x = 10;
    const int cx = 1;
    int tab[10] = {};
    vector<int> vec = {1, 2, 3};
    const vector<int> cvec = {1, 2, 3};

    auto a1 = x;
    static_assert(is_same<int, decltype(a1)>::value, "Error");

    auto a2 = &x;
    static_assert(is_same<int*, decltype(a2)>::value, "Error");

    const auto a3 = &x;
    static_assert(is_same<int* const, decltype(a3)>::value, "Error");

    auto* a4 = &x;
    static_assert(is_same<int*, decltype(a4)>::value, "Error");

    auto a5 = &cx;
    static_assert(is_same<const int*, decltype(a5)>::value, "Error");

    auto* a6 = &cx;
    static_assert(is_same<const int*, decltype(a6)>::value, "Error");

    int& rx = x;
    const int& crx = x;

    const auto& a7 = rx;
    static_assert(is_same<const int&, decltype(a7)>::value, "Error");

    auto a8 = crx;
    static_assert(is_same<int, decltype(a8)>::value, "Error");

    auto& a9 = crx;
    static_assert(is_same<const int&, decltype(a9)>::value, "Error");

    auto a10 = tab;
    static_assert(is_same<int*, decltype(a10)>::value, "Error");

    auto& a11 = tab;
    static_assert(is_same<int(&)[10], decltype(a11)>::value, "Error");

    auto a12 = vec.begin();
    static_assert(is_same<std::vector<int>::iterator, decltype(a12)>::value, "Error");

    auto a13 = vec.cbegin();
    static_assert(is_same<std::vector<int>::const_iterator, decltype(a13)>::value, "Error");

    auto a14 = vec.back();
    static_assert(is_same<int, decltype(a14)>::value, "Error");

    auto& a15 = cvec.back();
    static_assert(is_same<const int&, decltype(a15)>::value, "Error");

    const auto a16 = vec[0];
    static_assert(is_same<const int, decltype(a16)>::value, "Error");

    auto&& a17 = 7.5; // prvalue
    static_assert(is_same<double&&, decltype(a17)>::value, "Error");

    auto&& a18 = crx;
    static_assert(is_same<const int&, decltype(a18)>::value, "Error");

    auto&& a19 = vec.front();
    static_assert(is_same<int&, decltype(a19)>::value, "Error");

    auto&& a20 = vec.begin();
    static_assert(is_same<std::vector<int>::iterator&&, decltype(a20)>::value, "Error");
}

TEST_CASE("auto special case")
{
    auto il = {1, 2, 3}; // std::initializer_list<int>

    SECTION("Before C++17")
    {
        int x1(10);
        int x2{10};

        auto a1(10); // int
        auto a2{10}; // std::intializer_list<int>
        //auto a3{1, 2, 3}; // std::intializer_list<int>

        auto lst = {10}; // std::intializer_list<int>
    }

    SECTION("Before C++17")
    {
        int x1(10);
        int x2{10};

        auto a1(10); // int
        auto a2{10}; // int
        //auto a3{1, 2, 3}; // ill-formed

        auto lst = {10}; // std::intializer_list<int>
    }
}

TEST_CASE("{} with containers")
{
    int tab[10] = {1, 2, 3};
    std::vector<int> vec = {1, 2, 3};
    auto il = {1, 2, 3};
}

template <typename T>
class Container
{
    T* items_;
    size_t size_;
public:
    using iterator = T*;
    using const_iterator = const T*;

    Container(size_t size, const T& value)
        : items_{new T[size]}, size_{size}
    {
        std::fill(items_,  items_ + size_, value);
    }

    Container(std::initializer_list<T> il)  
        : items_{new T[il.size()]}, size_{il.size()}
    {
        std::copy(il.begin(), il.end(), items_);
    }

    Container(const Container&) = delete;
    Container& operator=(const Container&) = delete;

    size_t size() const
    {
        return size_;
    }

    iterator begin()
    {
        return items_;
    }

    iterator end()
    {
        return items_ + size_;
    }

    const_iterator begin() const
    {
        return items_;
    }

    const_iterator end() const
    {
        return items_ + size_;
    }

    ~Container() noexcept
    {
        delete[] items_;
    }
};

void print(const auto& coll, std::string_view desc)
{
    std::cout << desc << ": [ ";
    for(const auto& item : coll)
        std::cout << item << " ";
    std::cout << "]\n";
}

TEST_CASE("custom container")
{
    Container<int> container{1, 2, 3, 4};

    Container<int> data1(10, 1);
    print(data1, "data1");

    Container<int> data2{10, 1};
    print(data2, "data2");

    std::vector<int> vec1(10, 1);
    print(vec1, "vec1");

    std::vector<int> vec2{10, 1};
    print(vec2, "vec2");    
}

int foo()
{
    return 665;
}

struct X
{
    int value;
};

TEST_CASE("decltype")
{
    decltype(foo()) d1;

    std::map<int, std::string> dict;
    decltype(dict[0]) d2 = dict[0];

    X x{10};
    decltype(x.value) d3;
}

auto bar() -> int
{
    return 42;
}

template <typename T1, typename T2>
auto multiply(T1 a, T2 b) 
{
    if ( a > 5 )
        return 9.f;

    return static_cast<float>(a * b);
}

TEST_CASE("auto type deduction for function")
{
    auto r1 = multiply(1, 3.14);
}

template <typename TMap>
decltype(auto) get_value(TMap& map, typename TMap::key_type const& key)
{
    return map.at(key); 
}

template <typename TContainer>
decltype(auto) get_nth(TContainer& coll, size_t index)
{
    return coll[index]; 
}

TEST_CASE("auto vs. decltype(auto)")
{
    std::map<int, std::string> dict = { {1, "one"}, {2, "two"} };

    std::cout << get_value(dict, 1) << "\n";

    get_value(dict, 2) = "dwa";

    REQUIRE(dict[2] == "dwa");

    std::vector<bool> vb = {1, 1, 0, 0};

    get_nth(vb, 2) = true;

    REQUIRE(vb[2] == true);
}

struct Xx
{
    Xx(int ) {}
};

struct Yy
{
    Yy(Xx ) {}

    int foo() { return 42; }
};

TEST_CASE("")
{
    decltype(Yy(Xx(42)).foo()) var1;

    decltype(std::declval<Yy>().foo()) var2;
}

template <typename F>
auto create_populated(F f)
{
    std::vector<std::remove_cvref_t<decltype(f())>> coll;

    for(int i = 0; i < 10; ++i)
        coll.push_back(f());

    return coll;
}

auto create_generator(int seed)
{
    auto generator = [seed]() mutable { return ++seed; };

    return generator;
}

TEST_CASE("populate")
{
    std::vector vec = create_populated(create_generator(13));
    print(vec, "vec");
}

template <typename F, typename... Fs>
auto create_func_vec(F f, Fs... fs)
{
    std::vector<F> functions;
    functions.push_back(f);

    (..., functions.push_back(fs)); // Fold expression

    return functions;
}

void fa(int arg)
{
    std::cout << "fa(" << arg << ")\n";
}

void fb(int arg) noexcept
{
    std::cout << "fb(" << arg << ")\n";
}


TEST_CASE("vector function ptrs")
{
    //void (*ptr)(int) noexcept = fa;
    auto fs = create_func_vec(fa, fb);

    fs[0](42);
    fs[1](665);
}


class HeavyNMC
{
public:
    std::vector<int> vec;
    
    HeavyNMC() : vec(1'000'000)
    {
    }

    HeavyNMC(const HeavyNMC& source) = delete;
    HeavyNMC(HeavyNMC&& source) = delete;
};

class Heavy
{
public:
    std::vector<int> vec;
    
    Heavy() : vec(1'000'000)
    {
    }

    Heavy(const Heavy& source) : vec(source.vec)
    {   
        std::cout << "Heavy(const Heavy&)\n";
    }

    Heavy(Heavy&& source) : vec(std::move(source.vec))
    {
        std::cout << "Heavy(Heavy&&)\n";
    }

    // Heavy(const Heavy& source) = delete;
    
    // Heavy(Heavy&& source) = delete;
};

HeavyNMC create_heavy_rvo()
{
    return HeavyNMC(); // prvalue
}

void use(HeavyNMC h)
{
    std::cout << h.vec.size() << "\n";
}

Heavy create_heavy_nrvo()
{
    Heavy h;
    h.vec[100] = 665;

    return h; // lvalue
}

TEST_CASE("rvo & named-rvo")
{
    create_heavy_rvo().vec.push_back(15);
    
    Heavy&& h1 = create_heavy_nrvo();
    h1.vec.push_back(777);

    use(create_heavy_rvo());
}