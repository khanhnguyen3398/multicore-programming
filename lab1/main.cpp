#include <iostream>
#include "pthread.h"
#include "getopt.h"

using namespace std;

int main(int argc, char **argv)
{
	int num_threads;
	const char* arg;
	int c;

	ThreadSafeKVStore<std::string, int32_t> hashmap;
	ThreadSafeListenerQueue<int32_t> queue;
	

	opt = getopt(argc, argv, arg);
	for (int i = 0; i < num_threads; i++) {
		c = pthread_create()
	}
	pthread_create

}