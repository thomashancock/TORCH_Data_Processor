#ifndef THREADSAFEEVENTMAP_H
#define THREADSAFEEVENTMAP_H

// STD
#include <mutex>
#include <memory>
#include <map>

// LOCAL
#include "Event.hpp"

class ThreadSafeEventMap {
public:
	//! Constructor
	ThreadSafeEventMap();

	// Delete copy and move constructors and assignment operators
	ThreadSafeEventMap(const ThreadSafeEventMap& other) = delete;
	ThreadSafeEventMap& operator=(ThreadSafeEventMap other) = delete;
	ThreadSafeEventMap(const ThreadSafeEventMap&& other) = delete;
	ThreadSafeEventMap& operator=(ThreadSafeEventMap&& other) = delete;

private:
	mutable std::mutex m_mut; //!< Mutex used for thread safety

	std::map< unsigned int, std::unique_ptr<Event> > m_map;
};

#endif /* THREADSAFEEVENTMAP_H */
