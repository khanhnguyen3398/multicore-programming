#include <iostream>
 

template <typename K, typename V>
bool multimap<K,V>::insert(K& key, V& value)
{
	std::vector<V> key_values;
	bool inserted = find(key, key_values);
	// if already inserted, remove that key
	// to reinsert, if not then just insert
	if (inserted) {
		key_values.push_back(value);
		remove(key);
	}
	keys.push_back(key);
	key_values.push_back(value);
	orig_values.push_back(key_values);
	return true;
}

template <typename K, typename V>
bool multimap<K,V>::find(const K& key)
{
	int num = keys.size();
	for (int i = 0; i < num; i++) {
		if (keys.at(i) == key) {
			return true;
		}
	}
	return false;
}

template <typename K, typename V>
bool multimap<K,V>::find(const K& key, std::vector<V>& values)
{
	int num = keys.size();
	for (int i = 0; i < num; i++) {
		if (keys.at(i) == key) {
			values = orig_values.at(i);
			return true;
		}
	}
	return false;
}

template <typename K, typename V>
int multimap<K,V>::remove( K& key) 
{
	// find the key and then erase 
	// at that position and return size
	int num = keys.size();
	for (int i = 0; i < num; i++) {
		if (keys.at(i) == key) {
			keys.erase(keys.begin() +i);
			std::vector<V> values_target = orig_values.at(i);
			int value_sz = values_target.size();
			orig_values.erase(orig_values.begin() + i);
			return value_sz;
		}
	}
	return -1;
}
