#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <list>
#include <atomic>
#include "semaphore.h"

std::mutex mut;
std::condition_variable cond;

// In this implementation I'm using a mutex to lock and
// a condition variable to let thread wait/ be woken up
// when the queue is empty. 


template <typename T>
bool ThreadSafeListenerQueue<T>::push(const T element)
{

	std::unique_lock<std::mutex> lock(mut);
	queue.push_front(element);
	// all threads woken up when an element inserted
	cond.notify_all();
	lock.unlock();
	return true;

}

template <typename T>
bool ThreadSafeListenerQueue<T>::pop(T& element)
{
	std::unique_lock<std::mutex> lock(mut);
	if (queue.size() > 0) {
		element = *(queue.end());
		queue.pop_back();
		lock.unlock();
		return true;
	} else {
		lock.unlock();
		return false;
	}


}

template <typename T>
bool ThreadSafeListenerQueue<T>::listen(T& element)
{
	std::unique_lock<std::mutex> lock(mut);
	// thread wait if queue is empty
	while(queue.size() == 0) {
		cond.wait(lock);
	}
	element = queue.back();
	queue.pop_back();
	lock.unlock();
	return true;
}
