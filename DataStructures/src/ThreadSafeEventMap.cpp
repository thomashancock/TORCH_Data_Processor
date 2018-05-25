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
		// Mutex must be locked here to avoid deadlock!
		std::lock_guard<std::mutex> lk(m_mut);

		// for (auto it = m_map.begin(); it != m_map.end(); /* no increment */) {
		// 	returnVec.push_back(std::move(it->second));
		// 	ASSERT(nullptr == it->second);
		// 	m_map.erase(it++);
		// 	if (returnVec.back()->isComplete()) {
		// 		break;
		// 	}
		// }
		std::cout << "\t\t\tBeginning Write" << std::endl;
		// std::cout << "Map Size " << m_map.size() << std::endl;
		std::cout << "\t\t\tTracker Size " << m_eventTracker.size() << std::endl;

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

		std::cout << "Ending Write" << std::endl;
		if (m_eventTracker.empty()) {
			std::cout << "\tEvent Tracker Empty" << std::endl;
		} else {
			std::cout << "\tEvent tracker range " << m_eventTracker.front()->first << " - " << m_eventTracker.back()->first << std::endl;
		}
		// std::cout << "Map Size " << m_map.size() << std::endl;
		// std::cout << "Tracker Size " << m_eventTracker.size() << std::endl;
	}

	// returnVec will be moved due to RVO
	return returnVec;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
