#include <iostream>
#include <random>
#include <vector>
#include "multimap.hpp"


bool tester(multimap<int,int>& map);

int main()
{
	int key;
	int value;
	for (int i = 0 ; i < 10 ; i++) {
		multimap<int,int> test_map;
		bool test = tester(test_map);
		std::cout << "test " << i << ": ";
		if (test == 1 ){
			std::cout << "success\n";
		} else {
			std::cout << "fail \n";
		}
	}
return 0;
}

// function to test the map
bool tester( multimap<int,int>& map) 
{
	int key;
	int value;
	std::vector<int> check_list;
	for (int i  = 0; i < 100; i++) {
		std::default_random_engine key_generator;
		std::default_random_engine value_generator;
		std::uniform_int_distribution<int> distribution(0 , 200);
		key = distribution(key_generator);
		check_list.push_back(key);
		value = distribution(value_generator);
		map.insert(key, value);

	}
	bool found;
	// to check if the key-value pairs were added correctly
	for (int j = 0; j < 100; j++) {
		key = check_list.at(j);
		found = map.find(key);
		if (!found) {
			return false;
		}
	}
	return true;

}

