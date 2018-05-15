#ifndef EVENT_H
#define EVENT_H

#include <list>
#include <memory>
#include <map>

#include "Debug.hpp"
#include "Packet.hpp"

//! Container class for Packets
/*!
	This class is used to store Packets determined to correspond to the same event.

	\sa Packet
*/
class Event {
public:
	//! Constructor
	/*!
		TDC ID list will be used to check whether packets have been stored all TDCs.
	*/
	Event(
		const std::list<unsigned int>& tdcIDs
	);

	//! Attempts to add a packet to the Event object
	void addPacket(
		std::unique_ptr<Packet> packet //!< Pointer to the Packet being added
	);

	//! Checks whether a Packet is stored for all TDC IDs
	inline bool isComplete() const;

	//! Gets the Event ID for which the Event object is storing Packets
	inline unsigned int getEventID();

	//! Returns a stored Packet
	std::vector< std::unique_ptr<Packet> > removePackets();

	//! Prints information about the event
	void print() const;

private:
	unsigned int m_eventID = 0;      //!< Event ID which all stored packets should have
	bool m_isEventIDSet = false; //!< bool to tell whether Event ID has been set by the first stored Packet

	std::map< unsigned int, std::unique_ptr<Packet> > m_packetMap; //!< Map to store Packets by TDC ID
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Event::isComplete() const {
	// Check that a packet is stored for all entries in the map
	for (const auto& entry : m_packetMap) {
		if (nullptr == entry.second) {
			return false;
		}
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

#endif /* EVENT_H */
