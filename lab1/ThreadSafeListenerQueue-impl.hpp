#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <list>

template <typename T>
bool ThreadSafeListenerQueue<T>::push(const T element)
{
	std::unique_lock<std::mutex> lock(mut);
	queue.push_front(element);
	cond.notify_all();
	lock.unlock();
	return true;

}

template <typename T>
bool ThreadSafeListenerQueue<T>::pop(T& element)
{
	std::unique_lock<std::mutex> lock(mut);
	if (queue.size() > 0) {
		element = queue.end();
		queue.pop_back();
		lock.unlock();
		return true;
	} else {
		lock.unlock();
		return true;
	}


}

template <typename T>
bool ThreadSafeListenerQueue<T>::listen(T& element)
{
	std::unique_lock<std::mutex> lock(mut);
	while (queue.size() == 0) {
		cond.wait(mut, std::chrono::seconds(1));
	}
	T rvalue = queue.end();
	*element = rvalue;
	queue.pop_back;
	lock.unlock();
	return rvalue;
}
