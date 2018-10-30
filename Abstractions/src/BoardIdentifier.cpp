#include "BoardIdentifier.hpp"

// STD
#include <utility>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
BoardIdentifier::BoardIdentifier(
	const unsigned int chainID,
	const unsigned int deviceID
) :
	m_chainID(chainID),
	m_deviceID(deviceID)
{ }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator< (
	const BoardIdentifier& lhs,
	const BoardIdentifier& rhs
) {
	return std::make_pair(lhs.m_chainID, lhs.m_deviceID) < std::make_pair(rhs.m_chainID, rhs.m_deviceID);

	// return (lhs.m_deviceID < rhs.m_deviceID) ? true :
	//        ((lhs.m_chainID < rhs.m_chainID) ? true : false);

	// static auto getUniqueID = [] (const BoardIdentifier& id) {
	// 	return id.m_chainID * 1000 + id.m_deviceID;
	// };

	// return (getUniqueID(lhs) < getUniqueID(rhs));
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator== (
	const BoardIdentifier& lhs,
	const BoardIdentifier& rhs
) {
	return (lhs.m_chainID == rhs.m_chainID)&&(lhs.m_deviceID == rhs.m_deviceID);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::ostream& operator<<(
	std::ostream& os,
	const BoardIdentifier& identifier
) {
	os << "Board<ChainID: " << identifier.m_chainID << ", "
	   << "DeviceID: " << identifier.m_deviceID << ">";
	return os;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
