#include "BoardIdentifier.hpp"

// STD
// #include <iostream>
// #include <sstream>

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
// std::string BoardIdentifier::str() const {
// 	std::stringstream str;
// 	str << "IDentifier<ChainID: " << m_chainID << ", "
// 	    << "DeviceID: " << m_deviceID << ", "
// 	    << "TDCID: " << m_tdcID << ">";
// 	return str.str();
// }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator< (
	const BoardIdentifier& lhs,
	const BoardIdentifier& rhs
) {
	return (lhs.m_chainID < rhs.m_chainID) ? true :
	       (lhs.m_deviceID < rhs.m_deviceID) ? true : false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator> (
	const BoardIdentifier& lhs,
	const BoardIdentifier& rhs
) {
	return (lhs.m_chainID > rhs.m_chainID) ? true :
	       (lhs.m_deviceID > rhs.m_deviceID) ? true : false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator== (
	const BoardIdentifier& lhs,
	const BoardIdentifier& rhs
) {
	return (lhs.m_chainID == rhs.m_chainID)&&
	       (lhs.m_deviceID == rhs.m_deviceID);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::ostream& operator<<(
	std::ostream& os,
	const BoardIdentifier& identifier
) {
	os << "BoardIdentifier<ChainID: " << identifier.m_chainID << ", "
	   << "DeviceID: " << identifier.m_deviceID << ">";
	return os;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
