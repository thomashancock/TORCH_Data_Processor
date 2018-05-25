#include "ThreadSafeEventMap.hpp"

// STD
#include <utility>

using key = std::pair<unsigned int, unsigned int>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Static:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const std::function<bool(const key&, const key&)> ThreadSafeEventMap::s_comparator = [] (
	const key& k1,
	const key& k2
) -> bool {
	// Sort by Bunch ID if EventIDs match, otherwise by event ID
	// key.first = event ID
	// key.first = bunch ID
	return (k1.first == k2.first) ? k1.second < k2.second : k1.first < k2.first;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ThreadSafeEventMap::ThreadSafeEventMap(
	const std::list<unsigned int> tdcIDs
) :
	m_tdcIDs(tdcIDs),
	m_map(s_comparator)
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

	packet->print();

	// Check for event ID in map
	const auto packetKey = std::make_pair(packet->getEventID(),packet->getBunchID());
	// const auto eventID = packet->getEventID();
	auto found = m_map.find(packetKey);

	if (found == m_map.end()) {
		// If not found, create new event
		m_map.insert(std::make_pair(packetKey, std::make_unique<Event>(m_tdcIDs)));

		// Add packet to newly created event
		try {
			m_map.at(packetKey)->addPacket(std::move(packet));
		} catch (std::exception& e) {
			STD_ERR("Exception: " << e.what());
		}
	} else {
		// If event with correct ID is found, add packet
		found->second->addPacket(std::move(packet));
	}

	if (m_map.size() > 10) {
		WARNING("Map Bloat at " << m_map.size());
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
