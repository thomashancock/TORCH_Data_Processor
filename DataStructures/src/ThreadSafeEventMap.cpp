#include "ThreadSafeEventMap.hpp"

// STD
#include <utility>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ThreadSafeEventMap::ThreadSafeEventMap(
	const std::list<ReadoutIdentifier> readoutIDs
) :
	m_readoutIDs(std::move(readoutIDs))
{
	STD_LOG("ThreadSafeEventMap Constructor Called");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThreadSafeEventMap::addPacket(
	std::unique_ptr<Packet> packet
) {
	// Lock Mutex
	std::lock_guard<std::mutex> lk(m_mut);

	// Check for event ID in map
	const auto eventID = packet->getEventID();
	auto found = m_map.find(eventID);

	if (found == m_map.end()) {
		// If not found, create new event
		m_map.insert(std::make_pair(eventID, std::make_unique<Event>(m_readoutIDs)));
		m_eventTracker.push_back(m_map.find(eventID)); // Add event to tracker

		// Add packet to newly created event
		try {
			m_map.at(eventID)->addPacket(std::move(packet));
		} catch (std::exception& e) {
			STD_ERR("Exception: " << e.what());
		}
	} else {
		// If event with correct ID is found, add packet
		found->second->addPacket(std::move(packet));
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector< std::unique_ptr<Event> > ThreadSafeEventMap::popToComplete() {
	// Create vector to store events
	std::vector< std::unique_ptr<Event> > returnVec;
	ASSERT(returnVec.empty());

	// Check a complete event it stored in the list
	// If no complete event is stored, returnVec will be returned empty
	if (this->isCompleteStored()) {
		// Lock Mutex
		// Mutex must be locked here to avoid deadlock with isCompleteStored()!
		std::lock_guard<std::mutex> lk(m_mut);

		ASSERT(m_map.size() == m_eventTracker.size());

		while (!m_eventTracker.empty()) {
			// Access front entry of tracker
			auto& entry = m_eventTracker.front();

			// Add event pointed to by tracker entry to return vector
			returnVec.push_back(std::move(entry->second));
			ASSERT(nullptr == entry->second);

			// Erase the entry from the map and pop it from the tracker
			m_map.erase(entry);
			m_eventTracker.pop_front();

			// Check if the removed event was the complete event
			if (returnVec.back()->isComplete()) {
				break;
			}
		}

		ASSERT(m_map.size() == m_eventTracker.size());
	}

	// returnVec will be moved due to RVO
	return returnVec;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector< std::unique_ptr<Event> > ThreadSafeEventMap::dumpHalf() {
	if (this->isCompleteStored()) {
		WARNING("ThreadSafeEventMap::dumpHalf() called while complete event is stored!");
	}

	// Lock Mutex
	// Mutex must be locked here to avoid deadlock with isCompleteStored()!
	std::lock_guard<std::mutex> lk(m_mut);

	// Create vector to store events
	std::vector< std::unique_ptr<Event> > returnVec;
	ASSERT(returnVec.empty());

	ASSERT(m_map.size() == m_eventTracker.size());
	const auto nToDump = static_cast<unsigned int>(m_eventTracker.size()/2.0);
	ASSERT(nToDump > 0);

	for (unsigned int i = 0; i < nToDump; i++) {
		auto& entry = m_eventTracker.front();

		// Add event pointed to by tracker entry to return vector
		returnVec.push_back(std::move(entry->second));
		ASSERT(nullptr == entry->second);

		// Mark event as dumped
		returnVec.back()->setDumped();

		// Erase the entry from the map and pop it from the tracker
		m_map.erase(entry);
		m_eventTracker.pop_front();
	}

	// returnVec will be moved due to RVO
	return returnVec;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector< std::unique_ptr<Event> > ThreadSafeEventMap::dumpAll() {
	if (this->isCompleteStored()) {
		WARNING("ThreadSafeEventMap::dumpAll() called while complete event is stored!");
	}

	// Lock Mutex
	// Mutex must be locked here to avoid deadlock with isCompleteStored()!
	std::lock_guard<std::mutex> lk(m_mut);

	// Create vector to store events
	std::vector< std::unique_ptr<Event> > returnVec;
	ASSERT(returnVec.empty());

	ASSERT(m_map.size() == m_eventTracker.size());

	const auto nInBuffer = m_eventTracker.size();
	if (0 != nInBuffer) {
		for (unsigned int i = 0; i < nInBuffer; i++) {
			auto& entry = m_eventTracker.front();

			// Add event pointed to by tracker entry to return vector
			returnVec.push_back(std::move(entry->second));
			ASSERT(nullptr == entry->second);

			// Mark event as dumped
			returnVec.back()->setDumped();

			// Erase the entry from the map and pop it from the tracker
			m_map.erase(entry);
			m_eventTracker.pop_front();
		}

		ASSERT(m_map.size() == m_eventTracker.size());
		ASSERT(0 == m_eventTracker.size());
	}

	// returnVec will be moved due to RVO
	return returnVec;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
