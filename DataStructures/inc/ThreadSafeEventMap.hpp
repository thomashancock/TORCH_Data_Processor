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
	using key = std::pair<unsigned int, unsigned int>;

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

	//! Returns events up to the next complete event
	std::vector< std::unique_ptr<Event> > popToComplete();

private:
	const std::list<unsigned int> m_tdcIDs; //!< List of TDCs

	mutable std::mutex m_mut; //!< Mutex used for thread safety

	const static std::function<bool(const key&, const key&)> s_comparator; //!< Static comparator for map

	std::map< key, std::unique_ptr<Event>, decltype(s_comparator) > m_map; //!< Map of events
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool ThreadSafeEventMap::isCompleteStored() const {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Search for complete event in map
	for (const auto& entry : m_map) {
		if (entry.second->isComplete()) {
			return true;
		}
	}
	return false;
}

#endif /* THREADSAFEEVENTMAP_H */
