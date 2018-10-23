#include "ReadoutIdentifier.hpp"

// STD
#include <iostream>
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Public:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ReadoutIdentifier::ReadoutIdentifier(
	const BoardIdentifier& boardID,
	const unsigned int tdcID
) :
	BoardIdentifier(boardID),
	m_tdcID(tdcID)
{ }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadoutIdentifier::ReadoutIdentifier(
	const unsigned int chainID,
	const unsigned int deviceID,
	const unsigned int tdcID
) :
	ReadoutIdentifier(BoardIdentifier(chainID, deviceID), tdcID)
{ }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string ReadoutIdentifier::str() const {
	std::stringstream str;
	str << "IDentifier<ChainID: " << m_chainID << ", "
	    << "DeviceID: " << m_deviceID << ", "
	    << "TDCID: " << m_tdcID << ">";
	return str.str();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator< (
	const ReadoutIdentifier& lhs,
	const ReadoutIdentifier& rhs
) {
	return (lhs.m_chainID < rhs.m_chainID) ? true :
	       (lhs.m_deviceID < rhs.m_deviceID) ? true :
	       (lhs.m_tdcID < rhs.m_tdcID) ? true : false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator> (
	const ReadoutIdentifier& lhs,
	const ReadoutIdentifier& rhs
) {
	return (lhs.m_chainID > rhs.m_chainID) ? true :
	       (lhs.m_deviceID > rhs.m_deviceID) ? true :
	       (lhs.m_tdcID > rhs.m_tdcID) ? true : false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool operator== (
	const ReadoutIdentifier& lhs,
	const ReadoutIdentifier& rhs
) {
	return (lhs.m_chainID == rhs.m_chainID)&&
	       (lhs.m_deviceID == rhs.m_deviceID)&&
	       (lhs.m_tdcID == rhs.m_tdcID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
