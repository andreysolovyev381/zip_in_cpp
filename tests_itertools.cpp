//
// Created by Andrey Solovyev on 23/02/2023.
//

#include <gtest/gtest.h>
#include "itertools.hpp"
#include <vector>
#include <map>
#include <string>
#include <sstream>

TEST(BasicsItertools, Vector_String) {
	std::vector<int> v{ 1,2,3,4,5 };
	std::string s { "abcdefghhlk" };

	std::stringstream ss;

	for (auto const& [first, second] : itertools::zip(s, v)) {
		ss << first << ' ' << second << '\n';
	}
	std::string check {R"(a 1
b 2
c 3
d 4
e 5
)"};
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, Vector_Map) {
	using namespace std::string_literals;
	std::vector<int> v{ 1,2,3,4,5 };
	std::map<int, std::string> m {
		{1, "one"s},
		{2, "two"s},
		{3, "three"s},
		{4, "four"s},
		{5, "five"s},
	};

	std::stringstream ss;

	for (auto const& [i, pair] : itertools::zip(v, m)) {
		ss << i << ' ' << pair.first << ' ' << pair.second << '\n';
	}
	std::string check {R"(1 1 one
2 2 two
3 3 three
4 4 four
5 5 five
)"};
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, Vector_Map_String) {
	using namespace std::string_literals;
	std::vector<int> v{ 1,2,3,4,5 };
	std::map<int, std::string> m {
			{1, "one"s},
			{2, "two"s},
			{3, "three"s},
			{4, "four"s},
			{5, "five"s},
	};
	std::string s { "abcdefghhlk" };

	std::stringstream ss;

	for (auto const& [i, pair, ch] : itertools::zip(v, m, s)) {
		ss << i << ' ' << pair.first << ' ' << pair.second << ' ' << ch << '\n';
	}
	std::string check {R"(1 1 one a
2 2 two b
3 3 three c
4 4 four d
5 5 five e
)"};
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, OneContainer) {
	std::vector<int> v{ 1,2,3,4,5 };
	std::stringstream ss;

	for (auto const& [i] : itertools::zip(v)) {
		ss << i << '\n';
	}
	std::string check {R"(1
2
3
4
5
)"};
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, TwoContainers_OneEmpty) {
	std::vector<int> v{ 1,2,3,4,5 };
	std::string s;
	ASSERT_TRUE(s.empty());

	std::stringstream ss;

	for (auto const& [i, ch] : itertools::zip(v, s)) {
		ss << i << ' ' << ch << '\n';
	}

	std::string check;
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, NonContainers) {

	struct NotOkContainer { int value {42}; };
	[[maybe_unused]] NotOkContainer not_ok;

	[[maybe_unused]] std::vector<int> v{ 1,2,3,4,5 };

//	auto z = iterators::zip(v, not_ok); //doesn't compile
	//todo: add compile-time test
}

int main() {
	testing::InitGoogleTest();
	testing::GTEST_FLAG(color) = "yes";
	return RUN_ALL_TESTS();
}
