#ifndef THREADSAFELISTENERQUEUE_HPP
#define THREADSAFELISTENERQUEUE_HPP

#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>


template <typename T>
class ThreadSafeListenerQueue {
	public : 
		bool push(const T element);
		bool pop(T& element);
		bool listen(T& element);
		ThreadSafeListenerQueue();

	private :
		std::list<T> queue;
		bool empty;
};


#include "ThreadSafeListenerQueue-impl.hpp"

#endif
