#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <string_view>
#include <array>
#include <optional>
#include <variant>
#include <any>
#include <set>

#include "catch.hpp"

using namespace std::literals;

template <typename TCollection> 
void print(const TCollection& coll, std::string_view desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : coll)
		std::cout << item << " ";
	std::cout << "]\n";

	// printf(desc.data()); // C-API for string_view is a bug
}

TEST_CASE("string_view")
{
	print(std::vector{1, 2, 3}, "vec");

	std::string name = "vec";
	print(std::vector{5, 6, 7}, name);

	print(std::vector{6, 7, 8}, "data"s);
}

TEST_CASE("using string_view")
{
	auto sv1 = "text"sv;

	const char* ctext = "hello";
	std::string_view sv2 = ctext;

	std::string str = "text";
	std::string_view sv3 = str;

	REQUIRE(sv3 == "text"sv);
}

std::string_view may_be_dangling(std::string_view sv)
{
	return sv;
}

std::string safe(std::string_view sv)
{
	return std::string(sv);
}

TEST_CASE("beware")
{
	std::string_view sv1 = "text"s; // dangling pointer - UB

	std::string_view sv2 = may_be_dangling("text"s); // dangling pointer - UB
}

TEST_CASE("string_view is not null terminated string")
{
	std::string str = "abc def ghi";

	std::string_view token(str.data(), 3);

	REQUIRE(token == "abc");

	std::array buffer = {'a', 'b', 'c'};
	std::string_view token2(buffer.data(), buffer.size());

	REQUIRE(token == token2);
}

TEST_CASE("optional")
{
	std::optional<int> opt1;

	REQUIRE_FALSE(opt1.has_value());
	REQUIRE_THROWS_AS(opt1.value(), std::bad_optional_access);

	opt1 = 42;

	REQUIRE(opt1.has_value());
	REQUIRE(*opt1 == 42); // unsafe
	REQUIRE(opt1.value() == 42);

	std::optional<std::string> name = "Jan";
	name = std::nullopt;

	REQUIRE(name.value_or("(not set)") == "(not set)");

	name = "Adam";

	std::optional target = std::move(*name);
	REQUIRE(*target == "Adam");
	
	REQUIRE(name.has_value());  // optional has state - but the state is after move

	int x = 10;

	using RefInt = std::reference_wrapper<int>;
	std::optional<RefInt> opt_ref(x);

	REQUIRE(*opt_ref == 10);
}

[[nodiscard]] std::optional<int> to_int(std::string_view str)
{
	int value{};

	auto start = str.data();
	auto end = str.data() + str.size();

	if (const auto [pos_end, error_code] = std::from_chars(start, end, value); 
			error_code != std::errc{} || pos_end != end)
	{
		return std::nullopt;
	}

	return value;
}

TEST_CASE("to_int")
{
	SECTION("happy path")
	{
		auto number = to_int("42");		

		REQUIRE(number.value() == 42);
	}

	SECTION("sad path")
	{
		auto number = to_int("42abc"s);

		REQUIRE(number.has_value() == false);

		[[maybe_unused]] int x = 10;

		std::errc error = std::errc::connection_refused;
		
		switch (error)
		{
			case std::errc::connection_aborted:
				[[fallthrough]];
			case std::errc::connection_refused:
				std::cout << "Connection failed\n";
				break;
		}
	}
}

////////////////////////////////////////
TEST_CASE("any")
{
	std::any a1;

	REQUIRE(a1.has_value() == false);

	a1 = 3.14; // may allocate dynamic memory
	a1 = std::vector{1, 2, 3};
	a1 = "text"s;

	REQUIRE(std::any_cast<std::string>(a1) == "text"s);
	REQUIRE_THROWS_AS(std::any_cast<int>(a1), std::bad_any_cast); // copy of original value

	std::string* ptr_str = std::any_cast<std::string>(&a1); // using pointer to original value
	if (ptr_str)
	{
		std::cout << *ptr_str << "\n";
	}
}


////////////////////////////////////////
// std::variant

struct Data
{
	int value;

	Data(int value) : value{value}
	{}
};

TEST_CASE("std::variant")
{
	std::variant<int, double, std::string> v1; // always on stack

	REQUIRE(std::holds_alternative<int>(v1));
	REQUIRE(std::get<int>(v1) == 0);

	v1 = 3.14;
	v1 = 42;
	v1 = "text"s;

	REQUIRE(std::holds_alternative<std::string>(v1) == true);
	REQUIRE(v1.index() == 2);

	REQUIRE(std::get<std::string>(v1) == "text"s);
	REQUIRE_THROWS_AS(std::get<int>(v1), std::bad_variant_access);
	REQUIRE(std::get_if<int>(&v1) == nullptr);
}

TEST_CASE("when no defualt constructor available")
{
	std::variant<std::monostate, Data> vd;
}

struct Printer
{
	void operator()(int i) const 
	{
		std::cout << "int: " << i << "\n";
	}

	void operator()(double d) const 
	{
		std::cout << "double: " << d << "\n";
	}

	template <typename T>
		requires std::ranges::range<T>
	void operator()(const T& rng) const 
	{
		std::cout << "[ ";
		for(const auto& item : rng)
			std::cout << item << " ";
		std::cout << "]\n";
	}
};

template <typename... TLambdas>
struct overloaded : TLambdas...
{
	using TLambdas::operator()...;
};

// in C++17 we need deduction guide
template <typename... TLambdas>
overloaded(TLambdas...) -> overloaded<TLambdas...>;


TEST_CASE("visiting variant")
{
	std::variant<int, double, std::string, std::vector<int>> v1 = std::vector{1, 2, 3};

	Printer prn;

	std::visit(prn, v1);	

	const std::string prefix = " + ";

	auto local_printer = overloaded {
		[prefix](int i)  
		{
			std::cout << prefix << "int: " << i << "\n";
		},
		[prefix](double d) 
		{
			std::cout << prefix << "double: " << d << "\n";
		},
		[prefix](const auto& rng) 
		{	
			std::cout << prefix <<"[ ";
			for(const auto& item : rng)
				std::cout << item << " ";
			std::cout << "]\n";
		}
	};

	std::visit(local_printer, v1);

}

struct Lambda_7347823687236
{
	auto operator()(int a) const
	{
		return a * a;
	}
};

TEST_CASE("lambda expression")
{
	auto l = [](int a) { return a * a; };	

	decltype(l) another_l; // since C++20

	SECTION("is interpreted as")
	{
		auto l = Lambda_7347823687236();
	}
}

TEST_CASE("set")
{
	int data[] = {5, 7, 2, 8, 10, 11, 3};

	auto cmp_by_value = [](int* a, int* b) { return *a < *b; };

	//std::set<int*, decltype(cmp_by_value)> set_ptrs(cmp_by_value); // before C++20
	std::set<int*, decltype(cmp_by_value)> set_ptrs; // before C++20
	
	set_ptrs.insert(&data[0]);
	set_ptrs.insert(&data[1]);
	set_ptrs.insert(&data[2]);
	set_ptrs.insert(&data[3]);
	set_ptrs.insert(&data[4]);

	for(int* ptr : set_ptrs)
	{
		std::cout << *ptr << " ";
	}
	std::cout << "\n";
}

[[nodiscard]] std::variant<std::string, std::errc> load_from_file(const std::string& filename)
{
	if (filename == "secret")
		return std::errc::permission_denied;

	return std::string{"content of "} + filename;
}

TEST_CASE("using varaint to process returned values")
{
	std::visit(
		overloaded{
			[](const std::string& s) { std::cout << "Success: " << s << "\n"; },
			[](std::errc ec) { std::cout << "ERROR #" << static_cast<int>(ec) << "\n"; }
		},
		load_from_file("secret")
	);
}

struct Circle
{
	int radius;

	void draw() const
	{
		std::cout << "Drawing circle r: " << radius << "\n";
	}
};

struct Square
{
	int size;
 
	void draw() const
	{
		std::cout << "Drawing square s: " << size << "\n";
	}
};

struct Rectangle
{
	int width, height;

	void draw() const
	{
		std::cout << "Drawing rect w: " << width << ", h: " << height << "\n";
	}
};

TEST_CASE("static polymorphism")
{
	using Shape = std::variant<Circle, Square, Rectangle>;

	std::vector<Shape> shapes = { Circle{10}, Square{70}, Circle(30), Rectangle{100, 200} };

	auto drawer = [](const auto& shp) { shp.draw(); };

	for(const auto& shp : shapes)
	{
		//std::visit(drawer, shp);
		std::visit([](const auto& shp) { shp.draw(); }, shp);
	}
}