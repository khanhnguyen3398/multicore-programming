#ifndef THREADSAFEKVSTORE_HPP
#define THREADSAFEKVSTORE_HPP

#include "pthread.h"
#include <mutex>
#include <unordered_map>


template <typename K, typename V>
class ThreadSafeKVStore {
	public : 
		bool insert(const K key, const V value);
		bool accumulate(const K key, const V value);
		bool lookup(const K key, V& value);
		bool remove(const K key)

	private :
		std::unordered_map<K, V> hash_map;
		std::mutex lock;
};

#include "threadsafekvstore-impl.hpp"

#endif