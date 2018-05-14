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

	//! Quick check for if the packet is complete and consistent
	bool isGood() const { return isComplete()*isConsistent(); };

	//! Checks the packet contains both header and trailer information
	bool isComplete() const;

	//! Checks the packet contains consitent information
	/*!
		Checks the Header and Trailer TDC ID and event ID match.
		Ensures the TDC ID for each edge matches the header ID.
	*/
	bool isConsistent() const;

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
	unsigned int getEventID() const { return m_eventIDHeader; }
	//! Returns the stored bunch ID
	unsigned int getBunchID() const { return m_bunchID; }
	//! Returns the stored word count
	unsigned int getWordCount() const { return m_wordCount; }
	//! Returns the stored ROC value
	unsigned int getROCValue() const { return m_rocValue; }

	//! Returns the number of stored leading edges
	unsigned int getNLeadingEdges() const { return m_leadingEdges.size(); }
	//! Returns the number of stored trailing edges
	unsigned int getNTrailingEdges() const { return m_trailingEdges.size(); }

	//! Returns the channel ID of the egde requested
	Edge getEdge(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	) const;
	//! Returns the channel ID of the egde requested
	unsigned int getChannelID(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	) const;
		//! Returns the timestamp of the egde requested
	unsigned int getTimestamp(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	) const;
	//! Returns the fine timestamp of the egde requested
	unsigned int getFineTimestamp(
		const bool leading, //!< Get from leading edge of trailing egde
		const unsigned int edgeNo //!< Edge number requested
	) const;

	//! Write human-readable informaton about the packet
	void print() const;

private:
	const unsigned int m_rocValue; //!< ROC value which triggered the packet's creation

	// Header Info
	unsigned int m_tdcIDHeader = 0;   //!< The TDC ID decoded from the header
	unsigned int m_eventIDHeader = 0; //!< The event ID decoded from the header
	unsigned int m_bunchID = 0;       //!< The bunch ID decoded from the header

	// Trailer Info
	unsigned int m_tdcIDTrailer = 0;   //!< The TDC ID decoded from the trailer
	unsigned int m_eventIDTrailer = 0; //!< The event ID decoded from the trailer
	unsigned int m_wordCount = 0;      //!< The word count decoded from the trailer

	std::vector<Edge> m_leadingEdges;  //!< Vector storing all leading edge information
	std::vector<Edge> m_trailingEdges; //!< Vector storing all trailing edge information

	bool m_headerAdded = false;  //!< Stores whether a header line has been added
	bool m_trailerAdded = false; //!< Stores whether a trailer line has been added
};

#endif /* PACKET_H */
