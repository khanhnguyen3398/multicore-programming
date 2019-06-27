#include "pthread.h"
#include <mutex>
#include <iostream>

template <T>
ThreadSafeListenerQueue<T>::ThreadSafeListenerQueue()
{
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&empty, NULL);

}

template <T>
bool ThreadSafeListenerQueue<T>::push(const T element)
{
	pthread_mutex_lock(lock);
	queue.push_front(element);
	size++;
	pthread_cond_signal(empty);
	pthread_mutex_unlock(lock);
	return true;

}

template <T>
bool ThreadSafeListenerQueue<T>::pop(T& element)
{
	pthread_mutex_lock(lock);
	if (size > 0) {
		std::list<Tt>::iterator it  = queue.end();
		*element = *it;
	// does it copy or will it be disappeared?
		queue.erase(it);
		size--;
		pthread_mutex_unlock(lock);
		return true;
	} else {
		pthread_mutex_unlock(lock);
		return false;
	}


}

template <T>
bool ThreadSafeListenerQueue<T>::listen(T& element)
{
	pthread_mutex_lock(lock);
	while (size <= 0) {
		pthread_cond_wait(&cond, &lock);
	}
	std::list<Tt>::iterator it  = queue.end();
	*element = *it;
	// does it copy or will it be disappeared?
	queue.erase(it);
	size--;
	pthread_mutex_unlock(lock);
}
