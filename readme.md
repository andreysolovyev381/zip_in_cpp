### Zip in C++
#### Reasoning
I like small useful snippets, like this one. Let myself have an entertaining night while coding this exercise.
Although this one is a bit advanced, compared to the others. Iterator's big five is defined in the class:
- value_type
- reference
- difference
- pointer
- iterator_category

#### Usage
Pretty much straightforward, see the test file. Here is the most vivid example:
```c++
	std::vector<int> v{ 1,2,3,4,5 };
	std::map<int, std::string> m { {1, "one"s}, {2, "two"s}, {3, "three"s}, };
	std::string s { "abcdefghhlk" };

	std::stringstream ss;

	/// Here it is, see the for loop expr
	for (auto const& [i, pair, ch] : itertools::zip(v, m, s)) {
		ss << i << ' ' << pair.first << ' ' << pair.second << ' ' << ch << '\n';
	}
```
Works like a charm in 17 and 20.

#### Disclaimer 
Feel free to use it for your needs at your own risk. No guarantees of any kind is given :)

#### License
MIT License