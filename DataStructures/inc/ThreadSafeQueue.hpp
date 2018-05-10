#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

// STD
#include <queue>
#include <mutex>
#include <memory>

template <class T>
class ThreadSafeQueue {
public:
	//! Constructor
	ThreadSafeQueue();

	// Delete copy and move constructors and assignment operators
	ThreadSafeQueue(const ThreadSafeQueue& other) = delete;
	ThreadSafeQueue& operator=(ThreadSafeQueue other) = delete;
	ThreadSafeQueue(const ThreadSafeQueue&& other) = delete;
	ThreadSafeQueue& operator=(ThreadSafeQueue&& other) = delete;

	//! Is queue empty
	bool empty() const;

	//! Add a value to the queue
	void push(
		T value
	);

	//! Add a value to the queue
	T popFront();

private:
	mutable std::mutex m_mut; //!< Mutex used for thread safety

	std::queue<T> m_queue; //!< Base queue object
};

// Include function implementations
#include "ThreadSafeQueue.inl"

#endif /* THREADSAFEQUEUE_H */
