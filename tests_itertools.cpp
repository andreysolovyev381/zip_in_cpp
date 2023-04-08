//
// Created by Andrey Solovyev on 23/02/2023.
//

#include <gtest/gtest.h>

#include "itertools.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>


//#define WRONG_ITERATOR_COMPILE_FAILURE

TEST(BasicsItertools, IteratingOver_Vector_String) {
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
TEST(BasicsItertools, IteratingOver_DifferentIterTypes) {
	using namespace std::string_literals;

	struct TestStruct {
		std::map<int, std::string> const m {
				{1, "one"s}, {2, "two"s}, {3, "three"s}, {4, "four"s}, {5, "five"s},
		};
		auto begin() const { return m.cbegin(); }
		auto end() const { return m.cend(); }
	};
	TestStruct test_struct;
	std::vector<int> v{ 1,2,3,4,5 };
	std::string s { "abcdefghlk" };

	auto begin = itertools::zip(v.begin(), s.crbegin(), test_struct.begin());
	auto end = itertools::zip(v.end(), s.crend(), test_struct.end());

	std::stringstream ss;

	for (auto itb = begin, ite = end; itb != ite; ++itb ) {
		auto const &[normal, const_reverse, const_normal] = itb;
		auto const &[i, c] = const_normal;
		ss << normal << ' ' << const_reverse << ' ' << i << ' ' << c << '\n';
	}
	std::string check {R"(1 k 1 one
2 l 2 two
3 h 3 three
4 g 4 four
5 f 5 five
)"};
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, IteratingOver_Vector_Map) {
	using namespace std::string_literals;
	std::vector<int> v{ 1,2,3,4,5 };
	std::map<int, std::string> m {
		{1, "one"s},
		{2, "two"s},
		{3, "three"s}
	};

	std::stringstream ss;

	for (auto const& [i, pair] : itertools::zip(v, m)) {
		ss << i << ' ' << pair.first << ' ' << pair.second << '\n';
	}
	std::string check {R"(1 1 one
2 2 two
3 3 three
)"};
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, IteratingOver_Vector_Map_String) {
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
TEST(BasicsItertools, IteratingOver_OneContainer) {
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
TEST(BasicsItertools, IteratingOver_VectorBool) {
	std::vector<bool> v{ 1,1,0,1,0 };
	std::string s { "abcdefghhlk" };

	std::stringstream ss;
	ss << std::boolalpha;

	for (auto const& [b, c] : itertools::zip(v, s)) {
		ss << b << ' ' << c << '\n';
	}
	std::string check {R"(true a
true b
false c
true d
false e
)"};
	ASSERT_EQ(ss.str(), check);
}
TEST(BasicsItertools, IteratingOver_TwoContainers_OneEmpty) {
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
TEST(BasicsItertools, Distance) {
	using namespace std::string_literals;

	std::vector<int> v{ 1,2,3,4,5 };
	std::map<int, std::string> m { {1, "one"s}, {2, "two"s}, {3, "three"s} };

	auto begin = itertools::zip(v.begin(), m.crbegin());
	auto const adv {2u};
	auto second = std::next(begin, adv);

	auto dist = std::distance(begin, second);
	ASSERT_EQ(dist, adv);
}
TEST(BasicsItertools, Assignment) {
	std::vector<int> v{ 1,2,3,4,5 };
	std::string s {"abc"};

	auto begin = itertools::zip(v.begin(), s.rbegin());
	auto &[i, c] = begin;
	ASSERT_EQ(i, 1);
	ASSERT_EQ(c, 'c');
	i = 42;
	c = 'x';
	auto begin_new = itertools::zip(v.begin(), s.rbegin());
	auto &[i_new, c_new] = begin_new;
	ASSERT_EQ(i_new, 42);
	ASSERT_EQ(c_new, 'x');
}
TEST(BasicsItertools, MoveIteratorsCheck) {
	std::vector<std::vector<int>>
			v1{{ 1,2,3,4,5 }, {6, 7, 8} };
	ASSERT_TRUE(!v1.empty());
	ASSERT_TRUE(!v1.at(0).empty());

	auto v1_at_0_copy = v1.at(0);

	std::vector<std::string>
			s1{{ "0123456789_0123456789" }, {"0123456789_0123456789_0123456789"} };

	ASSERT_TRUE(!s1.empty());
	ASSERT_TRUE(!s1.at(0).empty());

	auto s1_at_0_copy = s1.at(0);

	auto begin = itertools::zip(std::make_move_iterator(v1.begin()), std::make_move_iterator(s1.begin()));
	auto &&[i, c] = std::move(begin);

//	ASSERT_TRUE(v1.at(0).empty()); //todo: fix required
	ASSERT_EQ(i, v1_at_0_copy);

//	ASSERT_TRUE(s1.at(0).empty()); //todo: fix required
	ASSERT_EQ(c, s1_at_0_copy);
}
TEST(BasicsItertools, Failure_NonContainers) {
	struct NotOkContainer { int value {42}; };
	[[maybe_unused]] NotOkContainer not_ok;

	[[maybe_unused]] std::vector<int> v{ 1,2,3,4,5 };

#ifdef WRONG_ITERATOR_COMPILE_FAILURE
	auto z = itertools::zip(v, not_ok); //fails to compile
#endif
}
TEST(BasicsItertools, Failure_BadIteratorCategory) {
	[[maybe_unused]] auto osit = std::ostream_iterator<int>{std::cout};
	[[maybe_unused]] std::vector<int> v{ 1,2,3,4,5 };
#ifdef WRONG_ITERATOR_COMPILE_FAILURE
	auto wit = itertools::zip(osit, v.begin()); //fails to compile
#endif
}
TEST(BasicsItertools, OperatorPreIncrement) {
	std::vector<int> v1{ 1,2 };
	std::vector<int> v2{ 2,3 };

	auto it = itertools::zip(v1.begin(), v2.begin());
	++it;
	auto [v1_elem, v2_elem] = it;
	ASSERT_EQ(v1_elem, 2);
	ASSERT_EQ(v2_elem, 3);
}
TEST(BasicsItertools, OperatorPostIncrement) {
	std::vector<int> v1{ 1,2 };
	std::vector<int> v2{ 2,3 };

	auto it = itertools::zip(v1, v2);
	it.begin()++;
	auto [v1_elem, v2_elem] = it.begin();
	ASSERT_EQ(v1_elem, 2);
	ASSERT_EQ(v2_elem, 3);
}
TEST(BasicsItertools, OperatorPreDecrement) {
	std::vector<int> v1{ 1,2 };
	std::vector<int> v2{ 2,3 };

	auto it = itertools::zip(v1.begin(), v2.begin());
	++it;
	--it;
	auto [v1_elem, v2_elem] = it;
	ASSERT_EQ(v1_elem, 1);
	ASSERT_EQ(v2_elem, 2);
}
TEST(BasicsItertools, OperatorPostDecrement) {
	std::vector<int> v1{ 1,2 };
	std::vector<int> v2{ 2,3 };

	auto it = itertools::zip(v1.begin(), v2.begin());
	++it;
	it--;
	auto [v1_elem, v2_elem] = it;
	ASSERT_EQ(v1_elem, 1);
	ASSERT_EQ(v2_elem, 2);
}
TEST(BasicsItertools, Failure_DecrementForForward) {
	std::vector<int> v{ 1,2 };
	std::unordered_map<int, int> m{ {1, 2}, {2,3} };

	auto it = itertools::zip(v.begin(), m.begin());
	++it;
#ifdef WRONG_ITERATOR_COMPILE_FAILURE
	it--; //fails to compile
#endif
}
TEST(BasicsItertools, OperatorAssignPlus) {
	std::vector<int> v{ 1,2,3 };
	std::map<int, int> m{ {1, 2}, {2,3}, {3,5} };

	auto it = itertools::zip(v.begin(), m.begin());
	it += 2;
	auto [v_elem, m_elem] = it;
	auto [key, value] = m_elem;
	ASSERT_EQ(v_elem, 3);
	ASSERT_EQ(key, 3);
	ASSERT_EQ(value, 5);
}
TEST(BasicsItertools, OperatorPlus) {
	std::vector<int> v{ 1,2,3 };
	std::map<int, int> m{ {1, 2}, {2,3}, {3,5} };

	auto it = itertools::zip(v.begin(), m.begin()) + 2;
	auto [v_elem, m_elem] = it;
	auto [key, value] = m_elem;
	ASSERT_EQ(v_elem, 3);
	ASSERT_EQ(key, 3);
	ASSERT_EQ(value, 5);
}
TEST(BasicsItertools, OperatorAssignMinus) {
	std::vector<int> v{ 1,2,3 };
	std::map<int, int> m{ {1, 2}, {2,3}, {3,5} };

	auto it = itertools::zip(v.begin(), m.begin());
	it += 2;
	it -= 2;
	auto [v_elem, m_elem] = it;
	auto [key, value] = m_elem;
	ASSERT_EQ(v_elem, 1);
	ASSERT_EQ(key, 1);
	ASSERT_EQ(value, 2);
}
TEST(BasicsItertools, OperatorMinus) {
	std::vector<int> v{ 1,2,3 };
	std::map<int, int> m{ {1, 2}, {2,3}, {3,5} };

	auto it = itertools::zip(v.begin(), m.begin()) + 2;
	it = it - 2;
	auto [v_elem, m_elem] = it;
	auto [key, value] = m_elem;
	ASSERT_EQ(v_elem, 1);
	ASSERT_EQ(key, 1);
	ASSERT_EQ(value, 2);
}
TEST(BasicsItertools, Failure_OperatorAssignMinusForNonBidirectional) {
	std::vector<int> v{ 1,2 };
	std::unordered_map<int, int> m{ {1, 2}, {2,3} };

	auto it = itertools::zip(v.begin(), m.begin());
	++it;
#ifdef WRONG_ITERATOR_COMPILE_FAILURE
	it -= 1; //fails to compile
#endif
}
TEST(BasicsItertools, Failure_OperatorMinusForNonBidirectional) {
	std::vector<int> v{ 1,2 };
	std::unordered_map<int, int> m{ {1, 2}, {2,3} };

	auto it = itertools::zip(v.begin(), m.begin());
	++it;
#ifdef WRONG_ITERATOR_COMPILE_FAILURE
	it = it - 1; //fails to compile
#endif
}

int main() {
	testing::InitGoogleTest();
	testing::GTEST_FLAG(color) = "yes";

	return RUN_ALL_TESTS();
}
