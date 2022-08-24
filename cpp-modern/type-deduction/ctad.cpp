#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "catch.hpp"

template <typename T1, typename T2>
struct ValuePair
{
	T1 fst;
	T2 snd;

	ValuePair(T1 f, T2 s) : fst{f}, snd{s}
	{}
};


TEST_CASE("CTAD")
{
	ValuePair<int, double> v1{42, 3.14};
}