#ifndef THREADSAFEKVSTORE_HPP
#define THREADSAFEKVSTORE_HPP

#include <pthread.h>
#include <mutex>
#include <map>
#include <shared_mutex>
#include <vector>

template <typename K, typename V>
class ThreadSafeKVStore {
	public : 
		bool insert(const K key, const V value);
		bool remove(const K key);
		V operator[] (int);
		void truncate(int n);
		K first_score();
		V first_elem();
		int size();

	private :
		std::multimap<K, V> hashmap;
		std::shared_mutex rwl;


};

#include "ThreadSafeKVStore-impl.hpp"

#endif