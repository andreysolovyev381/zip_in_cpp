### Zip in C++
#### Reasoning
I like small useful snippets, like this one. Let myself have an entertaining night while coding this exercise.

#### Usage
Pretty much strightforward, see the test file. Here is the most vivid example:
```
	std::vector<int> arr{ 1,2,3,4,5 };
	std::map<int, std::string> m {
			{1, "one"s},
			{2, "two"s},
			{3, "three"s},
			{4, "four"s},
			{5, "five"s},
	};
	std::string s { "abcdefghhlk" };

	std::stringstream ss;

	for (auto const& [first, pair, ch] : itertools::zip(arr, m, s)) {
		ss << first << ' ' << pair.first << ' ' << pair.second << ' ' << ch << '\n';
	}
```

#### Disclaimer 
Feel free to use it for your needs at your own risk. No guarantees of any kind is given :)