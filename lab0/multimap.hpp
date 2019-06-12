#ifndef MULTIMAP_HPP
#define MULTIMAP_HPP

#include <vector>


template <typename K, typename V>
class multimap {
	public : 
		bool insert( K& key, V& value);
		bool find(const K& key);
		bool find(const K& key, std::vector<V>& values);
		int remove( K& key);

	private :
		std::vector<K> keys;
		std::vector<std::vector<V> > orig_values;
};

#include "multimap-impl.hpp"

#endif