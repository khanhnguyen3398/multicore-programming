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

	private :
		std::list<T> queue;
		std::mutex mut;
  		std::condition_variable cond;
};

#include "ThreadSafeListenerQueue-impl.hpp"

#endif
