#ifndef THREADSAFEEVENTMAP_H
#define THREADSAFEEVENTMAP_H

// STD
#include <mutex>
#include <memory>
#include <map>
#include <list>
#include <vector>
#include <atomic>

// LOCAL
#include "Event.hpp"

class ThreadSafeEventMap {
	using eventMap = std::map< unsigned int, std::unique_ptr<Event> >;

public:
	//! Constructor
	ThreadSafeEventMap(
		const std::list<unsigned int> tdcIDs
	);

	// Delete copy and move constructors and assignment operators
	ThreadSafeEventMap(const ThreadSafeEventMap& other) = delete;
	ThreadSafeEventMap& operator=(ThreadSafeEventMap other) = delete;
	ThreadSafeEventMap(const ThreadSafeEventMap&& other) = delete;
	ThreadSafeEventMap& operator=(ThreadSafeEventMap&& other) = delete;

	//! Add a packet to the events
	void addPacket(
		std::unique_ptr<Packet> packet
	);

	//! Is a complete event stored?
	inline bool isCompleteStored() const;

	//! Returns whether the buffer is reaching capacity
	inline bool isBloated() const;

	//! Returns events up to the next complete event
	std::vector< std::unique_ptr<Event> > popToComplete();

	//! Returns first half of the events stored in the buffer
	std::vector< std::unique_ptr<Event> > dumpHalf();

	//! Returns all events in the buffer
	std::vector< std::unique_ptr<Event> > dumpAll();

private:
	const std::list<unsigned int> m_tdcIDs;

	mutable std::mutex m_mut; //!< Mutex used for thread safety

	eventMap m_map;

	std::list<eventMap::iterator> m_eventTracker;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool ThreadSafeEventMap::isCompleteStored() const {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Search for complete event in event tracker list
	for (const auto& entry : m_eventTracker) {
		if (entry->second->isComplete()) {
			return true;
		}
	}
	return false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline bool ThreadSafeEventMap::isBloated() const {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	ASSERT(m_map.size() == m_eventTracker.size());
	// Map is bloated if it contains more than 3000 events
	// Max is 4096, so this is ~3/4 full
	return m_map.size() > 3000;
}

#endif /* THREADSAFEEVENTMAP_H */
