#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>

// Implementation of ThreadSafeKVStore where I used a mutex
// and a unique lock to lock and unlock. 

std::mutex mut1;

template <typename K, typename V>
V ThreadSafeKVStore<K,V>::get_sum()
{
	V total;
	for ( auto it = hashmap.begin(); it != hashmap.end(); ++it ) {
		total += it->second;
	}
	return total;
}

template <typename K, typename V>
bool ThreadSafeKVStore<K,V>::insert(const K key, const V value)
{
	std::unique_lock<std::mutex> lock(mut1);
	bool found = false;
	for ( auto it = hashmap.begin(); it != hashmap.end(); ++it ) {
		if (it->first == key) {
			found = true;
			it->second = value;
			break;
		}
	}
	if (!found) {
		hashmap.insert(std::make_pair(key,value));
	}
	lock.unlock();
	return true;
}


template <typename K, typename V>
bool ThreadSafeKVStore<K,V>::accumulate(const K key, const V value)
{
	std::unique_lock<std::mutex> lock(mut1);
	bool found = false;
	for ( auto it = hashmap.begin(); it != hashmap.end(); ++it ) {
		if (it->first == key) {
			found = true;
			it->second = it->second + value;
			break;
		}
	}
	if (!found) {
		hashmap.insert (std::make_pair(key,value));
	}
	lock.unlock();
	return true;

}

template <typename K, typename V>
bool ThreadSafeKVStore<K,V>::lookup(const K key, V& value)
{
	bool found = false;
	std::unique_lock<std::mutex> lock(mut1);
	for ( auto it = hashmap.begin(); it != hashmap.end(); ++it ) {
		if (it->first == key) {
			found = true;
			value = it->second;
			break;
		}
	}
	lock.unlock();
	return found;

}

template <typename K, typename V>
bool ThreadSafeKVStore<K,V>::remove(const K key)
{
	std::unique_lock<std::mutex> lock(mut1);
	for ( auto it = hashmap.begin(); it != hashmap.end(); ++it ) {
		if (it->first == key) {
			hashmap.erase(key);
		}
	}
	lock.unlock();
	return true;
}