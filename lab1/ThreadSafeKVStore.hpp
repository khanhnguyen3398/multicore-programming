#ifndef THREADSAFEKVSTORE_HPP
#define THREADSAFEKVSTORE_HPP

#include <thread>
#include <mutex>
#include <unordered_map>

template <typename K, typename V>
class ThreadSafeKVStore {
	public : 
		bool insert(const K key, const V value);
		bool accumulate(const K key, const V value);
		bool lookup(const K key, V& value);
		bool remove(const K key);
		V get_sum();

	private :
		std::unordered_map<K, V> hashmap;

};

#include "ThreadSafeKVStore-impl.hpp"

#endif