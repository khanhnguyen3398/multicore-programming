#ifndef THREADSAFELISTENERQUEUE_HPP
#define THREADSAFELISTENERQUEUE_HPP

#include <mutex>
#include <list>
#include <condition_variable>


template <typename T>
class ThreadSafeListenerQueue {
	public : 
		bool push(const T element);
		bool pop(T& element);
		bool listen(T& element);
		int size();

	private :
		std::list<T> queue;
};


#include "ThreadSafeListenerQueue-impl.hpp"

#endif
