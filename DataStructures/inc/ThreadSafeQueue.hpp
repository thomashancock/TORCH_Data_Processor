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

#endif /* THREADSAFEQUEUE_H */
