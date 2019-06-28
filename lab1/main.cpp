#include <iostream>
#include <ctime>  
#include <cstdlib>
#include <thread>
#include "getopt.h"
#include <vector>
#include <random>
#include "ThreadSafeListenerQueue.hpp"
#include "ThreadSafeKVStore.hpp"

using namespace std;

void tester(ThreadSafeKVStore<std::string, int32_t> hashmap, ThreadSafeListenerQueue<int32_t> queue, int id);

int main(int argc, char **argv)
{
	int num_threads = 3;
	//opt = getopt(argc, argv, arg);

	ThreadSafeKVStore<std::string, int32_t> hashmap;
	ThreadSafeListenerQueue<int32_t> queue;

	std::vector<std::thread> threads;
	for (int i = 0; i < num_threads; i++) {
		threads.push_back(std::thread (tester, std::ref(hashmap), std::ref(queue), i));
	}

	int num_queue = num_threads;
	int32_t sum_all = 0;
	int32_t element;

	while (num_queue > 0 ) {
		queue.listen(element);
		sum_all += element;
		num_queue--;
	}

	for (int j = 0; j< num_threads; j++) {
		threads.at(j).join();
	}

	int32_t sum_check = hashmap.get_sum(); 
	if (sum_check == sum_all) {
		cout << "test succeeded \n" << endl;
	} else {
		cout << "error, thread not safe \n" << endl;
	}

}

void tester(ThreadSafeKVStore<std::string, int32_t> hashmap, ThreadSafeListenerQueue<int32_t> queue, int id)
{
	srand(id);
	std::default_random_engine generator;
  	std::uniform_int_distribution<int32_t> distribution(-256,256);
  	std::vector<string> keys_list;
  	int32_t keys_list_sz;
  	int32_t sum = 0;
	for(int i = 0; i < 10000; i++) {
		int operation = rand() % 10;
		if (operation < 2) {
			int32_t N = rand() % 500;
			string num = to_string(N);
			string key = "user" + num;
			int32_t value = distribution(generator);
			keys_list.push_back(key);
			hashmap.insert(key, value);
			sum += value;
		} else {
			keys_list_sz = keys_list.size();
			int pos = rand() % keys_list_sz;
			string key_lookup = keys_list.at(pos);
			hashmap.lookup(key_lookup);
		}
	}

	queue.push(sum);
}
