#ifndef PACKET_H
#define PACKET_H

#include <fstream>
#include <vector>

#include "Debug.hpp"
// #include "GlobalConfig.hpp"
// #include "DebugStreamManager.hpp"
#include "Edge.hpp"

//! Stores all the information relevant to a single packet of data
/*!
	Decodes data lines and stores the decoded information.
*/
class Packet {
public:
	//! Constructor
	Packet(
		const unsigned int rocValue //!< ROC value which triggered the packet's creation
	);
	//! Deconstructor
	~Packet();

	//! Quick check for if the packet is complete and consistent
	bool isGood() { return isComplete()*isConsistent(); };

	//! Checks the packet contains both header and trailer information
	bool isComplete();
	//! Checks the packet contains consitent information
	/*!
		Checks the Header and Trailer TDC ID and event ID match.
		Ensures the TDC ID for each edge matches the header ID.
	*/
	bool isConsistent();

	//! Adds information corresponding to a header word to the packet
	void addHeader(
		const unsigned int word //!< The word being added
	);
	//! Adds information corresponding to a trailer word to the packet
	void addTrailer(
		const unsigned int word //!< The word being added
	);
	//! Adds information corresponding to a data word to the packet
	void addDataline(
		const unsigned int word //!< The word being added
	);

	//! Returns the stored TDC ID
	unsigned int getTDCID() const { return m_tdcIDHeader; }
	//! Returns the stored event ID
	unsigned int getEventID() { return m_eventIDHeader; }
	//! Returns the stored bunch ID
	unsigned int getBunchID() { return m_bunchID; }
	//! Returns the stored word count
	unsigned int getWordCount() { return m_wordCount; }
	//! Returns the stored ROC value
	unsigned int getROCValue() { return m_rocValue; }

	//! Returns the number of stored leading edges
	unsigned int getNLeadingEdges() { return m_leadingEdges.size(); }
	//! Returns the number of stored trailing edges
	unsigned int getNTrailingEdges() { return m_trailingEdges.size(); }

	//! Returns the channel ID of the egde requested
	Edge getEdge(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	);
	//! Returns the channel ID of the egde requested
	unsigned int getChannelID(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	);
		//! Returns the timestamp of the egde requested
	unsigned int getTimestamp(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	);
	//! Returns the fine timestamp of the egde requested
	unsigned int getFineTimestamp(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	);

	//! Write human-readable informaton about the packet
	// void writeToOutfile(
	// 	std::shared_ptr<DebugStreamManager> dsm, //!< Output stream manager
	// 	const bool eventForm //!< Write in individual packet form or event form?
	// );

private:
	const unsigned int m_rocValue; //!< ROC value which triggered the packet's creation

	// Header Info
	unsigned int m_tdcIDHeader;   //!< The TDC ID decoded from the header
	unsigned int m_eventIDHeader; //!< The event ID decoded from the header
	unsigned int m_bunchID;       //!< The bunch ID decoded from the header

	// Trailer Info
	unsigned int m_tdcIDTrailer;   //!< The TDC ID decoded from the trailer
	unsigned int m_eventIDTrailer; //!< The event ID decoded from the trailer
	unsigned int m_wordCount;      //!< The word count decoded from the trailer

	std::vector<Edge> m_leadingEdges;  //!< Vector storing all leading edge information
	std::vector<Edge> m_trailingEdges; //!< Vector storing all trailing edge information

	bool m_headerAdded;  //!< Stores whether a header line has been added
	bool m_trailerAdded; //!< Stores whether a trailer line has been added
};

#endif /* PACKET_H */
