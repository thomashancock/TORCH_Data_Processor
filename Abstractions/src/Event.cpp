#include "Event.hpp"

// STD
#include <utility>

// LOCAL
#include "ErrorSpy.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Event::Event(
	const std::list<ReadoutIdentifier>& readoutIDs
) {
	ASSERT(readoutIDs.size() > 0);

	// Create slot per TDC id in the packet map
	for (const auto& id : readoutIDs) {
		m_packetMap.insert(std::make_pair(id,nullptr));
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Event::addPacket(
	std::unique_ptr<Packet> packet
) {
	ASSERT(packet != nullptr);

	// Check Event ID of packet matches event ID of event
	const auto packetEventID = packet->getEventID();
	if (true == m_isEventIDSet) {
		if (m_eventID != packetEventID) {
			STD_ERR("Packet event ID (" << packetEventID << ") does not match Event event ID (" << m_eventID << ")");
			return;
		}
	}

	// Attempt to add packet to event
	auto found = m_packetMap.find(packet->getReadoutID());

	// MORE TESTING
	// const auto tmp1 = ReadoutIdentifier(BoardIdentifier(1, 0), 1);
	// const auto tmp2 = ReadoutIdentifier(BoardIdentifier(1, 0), 1);
	// STD_LOG_VAR((tmp1 == tmp2));
	// STD_LOG_VAR( (!(tmp1 < tmp2) && !(tmp2 < tmp1)) );

	// exit(0);

	// TESTING
	// STD_LOG("Packet: " << packet->getReadoutID());
	// if (found != m_packetMap.end()) STD_LOG("Map:    " << found->first);
	// STD_LOG("---");
	//
	// static int count = 0;
	// count++;
	// if (count > 100) exit(0);

	if (found == m_packetMap.end()) {
		STD_ERR("Event found with unrequested Readout ID: " << packet->getReadoutID());
	} else if (found->second != nullptr) {
		ErrorSpy::getInstance().logError("Duplicate Packet Found",packet->getReadoutID());
	} else {
		found->second = std::move(packet);
		ASSERT(packet == nullptr);
	}

	// If Event ID is not set, set it
	if (false == m_isEventIDSet) {
		m_eventID = packetEventID;
		m_isEventIDSet = true;
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector< std::unique_ptr<Packet> > Event::removePackets() {
	std::vector< std::unique_ptr<Packet> > returnVec;

	for (auto& entry : m_packetMap) {
		if (entry.second != nullptr) {
			returnVec.push_back(std::move(entry.second));
		}
	}

	// returnVec moved due to RVO
	return returnVec;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Event::print() const {
	std::cout << "Event " << m_eventID << std::endl;
	if (!this->isComplete()) {
		std::cout << "\tIncomplete" << std::endl;
	}
	std::cout << "\tPackets: ";
	for (const auto& entry : m_packetMap) {
		if (nullptr != entry.second) {
			std::cout << entry.second->getReadoutID() << " ";
		}
	}
	std::cout << std::endl;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
