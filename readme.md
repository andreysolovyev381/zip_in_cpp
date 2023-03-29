## Zip in C++
### Reasoning
I like small useful snippets, like this one. Let myself have an entertaining night while coding this exercise.
Although this one is a bit advanced, compared to the others. Iterator's big five is properly defined in the class: value_type, reference, difference_type, pointer, iterator_category. So the other C++ code will consider this zip_iterator as an iterator as well.
Lots of concept (C++20) and SFINAE (C++17) guards will fail the attempts of enlightened user to get an iterator from void type.  

### Usage
Pretty much straightforward, see the test file. Here is the most vivid example:
```c++
	std::vector<int> v{ 1,2,3,4,5 };
	std::map<int, std::string> m { {1, "one"s}, {2, "two"s}, {3, "three"s}, };
	std::string s { "abcdefghhlk" };
	
	/// Here it is, see the for loop expr
	for (auto const& [i, pair, ch] : itertools::zip(v, m, s)) {
		std::cout << i << ' ' << pair.first << ' ' << pair.second << ' ' << ch << '\n';
	}
```

Another example, if to zip iterators, not containers

```c++
	struct TestStruct {
		std::map<int, std::string> m { {1, "one"s}, {2, "two"s}, {3, "three"s}, };
		auto begin() const { return m.begin(); }
		auto end() const { return m.end(); }
	};
	TestStruct test_struct;
	std::vector<int> v{ 1,2,3,4,5 };
	std::string s { "abcdefghlk" };

	/// Here it is, zipping input iterators
	auto begin = itertools::zip(v.cbegin(), s.crbegin(), test_struct.begin());
	auto end = itertools::zip(v.cend(), s.crend(), test_struct.end());

	for (auto itb = begin, ite = end; itb != ite; ++itb ) {
		auto const &[normal, const_reverse, const_normal] = itb;
		auto const &[i, c] = const_normal;
		std::cout << normal << ' ' << const_reverse << ' ' << i << ' ' << c << '\n';
	}

```

### Standard and Compilers
Works like a charm in 17 and 20. 
Is compiled by gcc, clang for Linux and something for Windows (thanks to Github Actions).

### Dependencies
Header only, STL only. 

### Disclaimer 
Feel free to use it for your needs at your own risk. No guarantees of any kind is given :)

### License
MIT License