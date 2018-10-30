#include "ReadoutIdentifier.hpp"

// STD
#include <utility>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ReadoutIdentifier::ReadoutIdentifier(
	const BoardIdentifier& boardID,
	const unsigned int tdcID
) :
	m_boardID(boardID),
	m_tdcID(tdcID)
{ }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator< (
	const ReadoutIdentifier& lhs,
	const ReadoutIdentifier& rhs
) {
	// return (!(lhs.m_boardID < rhs.m_boardID)) ? false :
	//        ((lhs.m_tdcID >= rhs.m_tdcID) ? false : true);

	return std::make_pair(lhs.m_boardID, lhs.m_tdcID) < std::make_pair(rhs.m_boardID, rhs.m_tdcID);

	// static auto getUniqueID = [] (const ReadoutIdentifier& id) {
	// 	return id.m_boardID.getChainID() * 1000000 + id.m_boardID.getDeviceID() * 1000 + id.m_tdcID;
	// };
	//
	// return (getUniqueID(lhs) < getUniqueID(rhs));

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator== (
	const ReadoutIdentifier& lhs,
	const ReadoutIdentifier& rhs
) {
	return (lhs.m_boardID == rhs.m_boardID)&&(lhs.m_tdcID == rhs.m_tdcID);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::ostream& operator<<(
	std::ostream& os,
	const ReadoutIdentifier& identifier
) {
	os << "Readout<" << identifier.m_boardID << ", "
	   << "TDCID: " << identifier.m_tdcID << ">";
	return os;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
