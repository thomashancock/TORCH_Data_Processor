#ifndef EVENT_H
#define EVENT_H

#include <initializer_list>
#include <fstream>
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
		// std::shared_ptr< std::vector<unsigned int> > tdcList //!< List of TDC IDs being used
	);

	//! Attempts to add a packet to the Event object
	/*!
		Returns true if the Packet was added succesfully, or false if the packet was rejected for some reason.
	*/
	bool addPacket(
		std::shared_ptr<Packet>& packet //!< Pointer to the Packet being added
	);

	//! Checks whether a Packet is stored for all TDC IDs
	bool isComplete() const;

	//! Gets the Event ID for which the Event object is storing Packets
	unsigned int getEventID();

	//! Returns a pointer to a stored Packet
	/*!
		Will return a NULL pointer if no packet is stored for the corresponding TDC ID.
	*/
	std::shared_ptr<Packet> getPacket(
		const unsigned int tdcID //!< TDC ID of the desired Packet
	);

	//! Writes information about the packets to the provided stream
	/*!
		Write information about the contained packets in a human readable for to facilitate debugging.
	*/
	// void writeToOutfile(
	// 	std::shared_ptr<DebugStreamManager> dsm //!< Pointer to DebugStreamManager
	// );

private:
	//! Checks the TDC ID of the packet is valid
	/*!
		Matches the Packet TDC ID to the stored list and returns false if no matches are found.
	*/
	bool isTDCIDValid(
		const Packet* packet //!< Pointer to the packet being checked
	);

private:
	const std::shared_ptr< std::vector<unsigned int> > m_tdcList; //!< List of valid TDC IDs

	unsigned int m_eventID;      //!< Event ID which all stored packets should have
	bool m_isEventIDSet; //!< bool to tell whether Event ID has been set by the first stored Packet

	std::map<unsigned int,std::shared_ptr<Packet> > m_packetMap; //!< Map to store Packets by TDC ID
};

#endif /* EVENT_H */
