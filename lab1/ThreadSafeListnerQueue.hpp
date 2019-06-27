#ifndef THREADSAFELISTENERQUEUE_HPP
#define THREADSAFELISTENERQUEUE_HPP

#include "pthread.h"
#include <mutex>

template <T>
class ThreadSafeListenerQueue {
	public : 
		ThreadSafeListnerQueue();
		bool push(const T element);
		bool pop(T& element);
		bool listen(T& element);

	private :
		std::list<T> queue;
		int size;
		pthread_mutex_t lock;
		pthread_cond_t empty;
};

#include "ThreadSafeListenerQueue-impl.hpp"

#endif
