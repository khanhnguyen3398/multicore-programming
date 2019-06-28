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

static void tester(ThreadSafeKVStore<std::string, int32_t> hashmap, ThreadSafeListenerQueue<int32_t> queue, int id);

int main(int argc, char **argv)
{
	int num_threads = 5;
	//opt = getopt(argc, argv, arg);

	ThreadSafeKVStore<std::string, int32_t> hashmap;
	ThreadSafeListenerQueue<int32_t> queue;

	std::vector<std::thread> threads;
	for (int i = 0; i < num_threads; i++) {
		threads.push_back(std::thread ([=] { tester(hashmap, queue, i); }));
	}


	int num_queue = num_threads;
	int32_t sum_all = 0;
	int32_t element;

	while (num_queue > 0 ) {
		cout << "listening " << endl;
		queue.listen(element);
		cout << "element listend is " << element << endl;
		sum_all += element;
		num_queue--;
	}
	
	cout << "checkpoint 1 \n" << endl;

	cout << "checkpoint 2 \n" << endl;

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

static void tester(ThreadSafeKVStore<std::string, int32_t> hashmap, ThreadSafeListenerQueue<int32_t> queue, int id)
{
	srand(id);
	std::default_random_engine generator;
  	std::uniform_int_distribution<int32_t> distribution(-256,256);
  	std::vector<string> keys_list;
  	std::vector<int32_t> values_list;
  	int32_t keys_list_sz;
  	int32_t sum = 0;


	cout << "checkpoint 2 \n" << endl;

	for(int i = 0; i < 10000; i++) {
		int operation = rand() % 10;
		if (operation < 2) {
			int32_t N = rand() % 500;
			string num = to_string(N);
			string key = "user" + num;
			int32_t value = distribution(generator);
			keys_list.push_back(key);
			values_list.push_back(value);
			hashmap.insert(key, value);
			sum += value;
		} else {
			keys_list_sz = keys_list.size();
			if (keys_list_sz) {
				int pos = rand() % keys_list_sz;
				hashmap.lookup(keys_list.at(pos), values_list.at(pos));
			}
		}
	}

	cout << "sum is " << sum << endl;
	queue.push(sum);
}
