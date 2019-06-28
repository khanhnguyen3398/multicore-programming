#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <list>

std::mutex mut;
std::condition_variable cond;

using namespace std;

template <typename T>
ThreadSafeListenerQueue<T>::ThreadSafeListenerQueue()
{
	empty = true;
}

template <typename T>
bool ThreadSafeListenerQueue<T>::push(const T element)
{
	std:: cout << "pushed called " << std::endl;
	std::unique_lock<std::mutex> lock(mut);
	queue.push_front(element);
	empty = false;
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
	while (empty) {
		std::cout << "waiting" << std::endl;
		cond.wait(lock);
	}
	std::cout << "stopped waiting " << std::endl;
	element = *(queue.end());
	queue.pop_back();
	lock.unlock();
	return true;
}
