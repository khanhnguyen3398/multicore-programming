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

static void tester(ThreadSafeKVStore<std::string, int32_t>& hashmap, ThreadSafeListenerQueue<int32_t>& queue, int id);

int main(int argc, char **argv)
{
	// get the number of threads from command line
	int num_threads;
	char* argument;
    int c ;
    while( ( c = getopt (argc, argv, "n:") ) != -1 ) 
    {
        switch(c)
        {
            case 'n':
                if(optarg) 
                	argument = optarg;
                break;
        }
    }
    num_threads = int(*argument) - 48;


	ThreadSafeKVStore<std::string, int32_t> hashmap;
	ThreadSafeListenerQueue<int32_t> queue;

	// spawn the threads and pass the KVstore and queue into those threads
	// each thread calls tester function 
	std::vector<std::thread> threads;
	for (int i = 0; i < num_threads; i++) {
		threads.push_back(std::thread(tester, std::ref(hashmap), std::ref(queue), i));
	}

	int num_queue = num_threads;
	int32_t sum_all = 0;
	int32_t element;

	// main thread listens onto the queue for any sum pushed onto queue
	// by each threads spawned above
	while (num_queue > 0 ) {
		queue.listen(element);
		sum_all += element;
		num_queue--;
	}

	// join the threads
	for (int j = 0; j< num_threads; j++) {
		threads.at(j).join();
	}

	// main function calls a public class function to iterate
	// through the underlying structure to find sum
	int32_t sum_check = hashmap.get_sum(); 

	cout << "sum from main is : " << sum_check << endl;
	cout << "sum from queue is : " << sum_all << endl;


	if (sum_check == sum_all) {
		cout << "test succeeded \n" << endl;
	} else {
		cout << "error, thread not safe \n" << endl;
	}

}

static void tester(ThreadSafeKVStore<std::string, int32_t>& hashmap, ThreadSafeListenerQueue<int32_t>& queue, int id)
{
	// every thread has a different seed for random generator
	srand(id);
	std::default_random_engine generator;
  	std::uniform_int_distribution<int32_t> distribution(-256,256);
  	std::vector<string> keys_list;
  	std::vector<int32_t> values_list;
  	int32_t keys_list_sz;
  	int32_t sum = 0;

  	// each thread does iteration 10000 times of the test,
  	// with 20% probability of inserting (thus the < 2) 
  	// and 80% probability of lookup

	for(int i = 0; i < 10000; i++) {
		int operation = rand() % 10;
		if (operation < 2) {
			int32_t N = rand() % 500;
			string num = to_string(N);
			string key = "user" + num;
			int32_t value = distribution(generator);
			keys_list.push_back(key);
			values_list.push_back(value);
			hashmap.accumulate(key, value);
			sum += value;
		} else {
			keys_list_sz = keys_list.size();
			if (keys_list_sz) {
				int pos = rand() % keys_list_sz;
				bool lookup_res = hashmap.lookup(keys_list.at(pos), values_list.at(pos));
				if (!lookup_res) {
					cerr << "lookup problem! key-value pair deleted " << endl;
				}
			}
		}
	}

	queue.push(sum);
}
