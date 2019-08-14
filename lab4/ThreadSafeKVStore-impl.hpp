#include <iostream>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <map>

// Implementation of ThreadSafeKVStore where I used a mutex
// and a unique lock to lock and unlock. 


template <typename K, typename V>
V ThreadSafeKVStore<K,V>::first_elem()
{
	rwl.lock_shared();
		V value = (*(hashmap.begin())).second;
	rwl.unlock_shared();
	return value;
}

template <typename K, typename V>
K ThreadSafeKVStore<K,V>::first_score()
{
	rwl.lock_shared();
		K value = (*(hashmap.begin())).first;
	rwl.unlock_shared();
	return value;
}

template <typename K, typename V>
int ThreadSafeKVStore<K,V>::size()
{
	int size = 0;
	rwl.lock_shared();
	size = hashmap.size();
	rwl.unlock_shared();
	return size;
}

template <typename K, typename V>
bool ThreadSafeKVStore<K,V>::insert(const K key, const V value)
{
	rwl.lock();
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
	rwl.unlock();
	return true;
}


template <typename K, typename V>
bool ThreadSafeKVStore<K,V>::remove(const K key)
{
	rwl.lock();
	for ( auto it = hashmap.begin(); it != hashmap.end(); ++it ) {
		if (it->first == key) {
			hashmap.erase(key);
		}
	}
	rwl.unlock();
	return true;
}

/*operator[n]: Thread-safely, select and return the nth element in the ordered
multimap, clamping the indices to the length of the multimap (i.e., if n>=the number
of items in the multimap, return the last one). This operator need be only an rval, not
an lval.
*/


template <typename K, typename V>
V ThreadSafeKVStore<K,V>::operator [] (int n)
{
	int pos = 0;
	V element;
	rwl.lock_shared();
	if (n >= hashmap.size()) {
		auto iter = hashmap.end();
		iter--;
		element = (*iter).second;
		rwl.unlock_shared();
		return element;
	}
	if (n == 0) {
		auto iter1 = hashmap.begin();
		element = (*iter1).second;
			rwl.unlock_shared();
		return element;
	}
	for ( auto it = hashmap.begin(); it != hashmap.end(); ++it ) {
		if (pos == n - 1) {
			//copy over value
			element = (*it).second;
			break;
		}
		pos++;
	}
	rwl.unlock_shared();
	// unlock
	return element;
}

//truncate(n): Thread-safely truncate the multimap to the first n k-v pairs.

template <typename K, typename V>
void ThreadSafeKVStore<K,V>::truncate(int n)
{	            
	//std::cout << "trunccccccc \n";

	rwl.lock();
	int pos = 0;
	int size = hashmap.size();
	auto it = hashmap.end();
		//std::cout << "endddd \n";
	//auto next = it;

	//for (int j = 0; j <= n; j++) {
			//std::cout << "it++++ \n";
		//it++;
	//}
	int diff = size - n;
	it--;

	for (int i = 0; i < diff; i++ ){
			//std::cout << "erase \n";
		it = hashmap.erase(it);
			//std::cout << "eraseddddd \n";
		it--;
	}

	rwl.unlock();
	            //std::cout << "truncateddddd \n";

	/* or

	iterator it = hashmap.end();

	rwl.lock();
	int pos = 0;
	int size = hashmap.size();
	iterator it = hashmap.begin();
	iterator next;

	for (int j = 0; j <= n; j++) {
		it++;
	}

	for (int i = n; i < size; i++ ){
		next = it++;
		it = hashmap.erase(it);
		it = next;i
	}

	rwl.unlock();


	std::unique_lock<std::mutex> lock(mut1);
	for (auto it = m.begin(); it != m.end(); ) {
   		if (pos >= n) {
       		it = m.erase(it);
       		pos++;
       		it++;
   		}else {
       		it++;
       		pos++;
       	}
    }
    */
}

