#include "ThreadSafeEventMap.hpp"

// STD
#include <utility>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ThreadSafeEventMap::ThreadSafeEventMap(
	const std::list<unsigned int> tdcIDs
) :
	m_tdcIDs(tdcIDs)
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
		m_map.insert(std::make_pair(eventID, std::make_unique<Event>(m_tdcIDs)));

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
		// Mutex must be locked here to avoid deadlock!
		std::lock_guard<std::mutex> lk(m_mut);

		for (auto it = m_map.begin(); it != m_map.end(); /* no increment */) {
			returnVec.push_back(std::move(it->second));
			ASSERT(nullptr == it->second);
			m_map.erase(it++);
			if (returnVec.back()->isComplete()) {
				break;
			}
		}
	}

	// returnVec will be moved due to RVO
	return returnVec;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
