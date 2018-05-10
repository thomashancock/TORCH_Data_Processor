// LOCAL
#include "Debug.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <class T>
ThreadSafeQueue<T>::ThreadSafeQueue() {
	ASSERT(m_queue.empty());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class T>
inline bool ThreadSafeQueue<T>::empty() const {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Return queue empty flag
	return m_queue.empty();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class T>
inline void ThreadSafeQueue<T>::push(
	T value
) {
	// Lock mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Add value to queue
	m_queue.push(std::move(value));
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class T>
inline T ThreadSafeQueue<T>::popFront() {
	// Lock mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Ensure queue is not empty
	ASSERT(!m_queue.empty());

	// Get value at front of queue
	auto value = std::move(m_queue.front());

	// Pop value from queue
	m_queue.pop();

	// Return value
	return value;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
