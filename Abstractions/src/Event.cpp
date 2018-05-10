#include "Event.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Event::Event(
	// std::shared_ptr< std::vector<unsigned int> > tdcList
) :
// m_tdcList(tdcList),
m_eventID(0),
m_isEventIDSet(false)
{
	// ASSERT(tdcList != nullptr);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool Event::addPacket(
	std::shared_ptr<Packet>& packet
) {
	// STD_LOG("Adding Packet to " << m_eventID);
	if (true == m_isEventIDSet) {
		if (m_eventID != packet->getEventID()) {
			STD_ERR("Packet event ID does not match Event event ID");
			return false;
		}
	} else {
		m_eventID = packet->getEventID();
		m_isEventIDSet = true;
	}

	if (isTDCIDValid(packet.get())) {
		// Store packet with tdcID as key
		const unsigned int tdcID = packet->getTDCID();
		if (!m_packetMap.insert(
			std::pair<unsigned int,std::shared_ptr<Packet> > (tdcID,packet)
			).second) {
			// WARNING("Event: " << m_eventID << ": TDC ID " << tdcID << " is duplicate");
			return false;
		}
	} else {
		STD_ERR("Packet has invalid TDC ID: " << packet->getTDCID());
		return false;
	}
	return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool Event::isComplete() const {
	// As TDC ID is checked upon entry, and only one of each TDC ID can be stored, if the TDC list size and map size are the same then a packet exists for all TDCs
	if (m_packetMap.size() != m_tdcList->size()) {
		return false;
	}
	return true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int Event::getEventID() {
	if (false == m_isEventIDSet) {
		STD_ERR("EventID has not been set");
	}
	return m_eventID;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::shared_ptr<Packet> Event::getPacket(
	const unsigned int tdcID
) {
	auto search = m_packetMap.find(tdcID);
	if (search != m_packetMap.end()) {
		return search->second;
	} else {
		return NULL;
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// void Event::writeToOutfile(
// 	std::shared_ptr<DebugStreamManager> dsm
// ) {
// 	for (auto& tdcID : *m_tdcList) {
// 		auto itr = m_packetMap.find(tdcID);
// 		if (itr == m_packetMap.end()) {
// 			// If no packet exists for the TDC number, print padding
// 			dsm->getStream("eventLevel") << "-00."; // Readout Board number
// 			if (tdcID < 10) dsm->getStream("eventLevel") << 0; // Print padding
// 			dsm->getStream("eventLevel") << tdcID;
// 			dsm->getStream("eventLevel") << " " << m_eventID << " 0 0" << std::endl;
//
// 			dsm->getStream("eventLevel") << "-00."; // Readout Board number
// 			if (tdcID < 10) dsm->getStream("eventLevel") << 0; // Print padding
// 			dsm->getStream("eventLevel") << tdcID;
// 			dsm->getStream("eventLevel") << " 1 0 0 1 2" << std::endl;
// 		} else {
// 			// If packet exists, write it to the output stream
// 			itr->second->writeToOutfile(dsm,true);
// 		}
// 	}
// }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Event::isTDCIDValid(
	const Packet* packet
) {
	// const unsigned int packetTDCID = packet->getTDCID();
	// for (auto& tdcID : *m_tdcList) {
	// 	if (packetTDCID == tdcID) {
	// 		return true;
	// 	}
	// }
	return false;
}
