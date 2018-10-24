#include "ReadoutIdentifier.hpp"

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
	return (lhs.m_boardID < rhs.m_boardID) ? true :
	       (lhs.m_tdcID < rhs.m_tdcID) ? true : false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator> (
	const ReadoutIdentifier& lhs,
	const ReadoutIdentifier& rhs
) {
	return (lhs.m_boardID > rhs.m_boardID) ? true :
	       (lhs.m_tdcID > rhs.m_tdcID) ? true : false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator== (
	const ReadoutIdentifier& lhs,
	const ReadoutIdentifier& rhs
) {
	return (lhs.m_boardID == rhs.m_boardID)&&
	       (lhs.m_tdcID == rhs.m_tdcID);
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
