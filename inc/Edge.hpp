#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "Debug.hpp"

//! Container class for Edge information
/*!
	Stores all information relevant to a single leading or trailing edge
*/
class Edge {
public:
	//! Constructor
	Edge(
		const unsigned int tdcID,
		const unsigned int channelID,
		const unsigned int timestamp
	);
	//! Deconstructor
	~Edge();

	//! Getter for the edge TDC ID
	unsigned int getTDCID() { return m_tdcID; };
	//! Getter for the edge channel ID
	unsigned int getChannelID() { return m_channelID; };
	//! Getter for the edge timestamp
	unsigned int getTimestamp() const { return m_timestamp; };
	//! Getter for the edge fine timestamp
	unsigned int getFineTimestamp() { return m_fineTimestamp; };

	//! Operator overload to allow edges to be sorted by time
	bool operator< (
		const Edge &edge1
	);

private:
	const unsigned int m_tdcID = 0;         //!< The TDC ID of the edge
	const unsigned int m_channelID = 0;     //!< The channel ID of the edge
	const unsigned int m_timestamp = 0;     //!< The timestamp of the edge
	const unsigned int m_fineTimestamp = 0; //!< The fine timestamp of the edge
};

#endif /* EDGE_H */
