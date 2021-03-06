#include "Edge.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Edge::Edge(
	const unsigned int tdcID,
	const unsigned int channelID,
	const unsigned int timestamp
) :
m_tdcID(tdcID),
m_channelID(channelID),
m_timestamp(timestamp),
#ifdef VHR_MODE
m_fineTimestamp((timestamp >> 2)%256)
# else
m_fineTimestamp(timestamp%256)
#endif /* VHR_MODE */
{ }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool Edge::operator< (
	const Edge& other
) const {
	return this->getTimestamp() < other.getTimestamp();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
