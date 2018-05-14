#ifndef EDGE_H
#define EDGE_H

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

	//! Getter for the edge TDC ID
	unsigned int getTDCID() const { return m_tdcID; };
	//! Getter for the edge channel ID
	unsigned int getChannelID() const { return m_channelID; };
	//! Getter for the edge timestamp
	unsigned int getTimestamp() const { return m_timestamp; };
	//! Getter for the edge fine timestamp
	unsigned int getFineTimestamp() const { return m_fineTimestamp; };

	//! Operator overload to allow edges to be sorted by time
	bool operator< (
		const Edge& other
	) const;

private:
	const unsigned int m_tdcID;         //!< The TDC ID of the edge
	const unsigned int m_channelID;     //!< The channel ID of the edge
	const unsigned int m_timestamp;     //!< The timestamp of the edge
	const unsigned int m_fineTimestamp; //!< The fine timestamp of the edge
};

#endif /* EDGE_H */
