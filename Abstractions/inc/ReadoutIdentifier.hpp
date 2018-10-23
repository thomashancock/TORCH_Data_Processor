#ifndef READOUTIDENTIFIER_H
#define READOUTIDENTIFIER_H

// STD
#include <string>
#include <array>

// LOCAL
#include "BoardIdentifier.hpp"

//! Class used to uniquely idenfity combinations of readout informtion
/*!

*/
class ReadoutIdentifier : public BoardIdentifier {
public:
	ReadoutIdentifier(
		const BoardIdentifier& boardID,
		const unsigned int tdcID
	);

	ReadoutIdentifier(
		const unsigned int chainID,
		const unsigned int deviceID,
		const unsigned int tdcID
	);

	//! Getter for Chain ID
	auto getChainID() const { return m_chainID; }
	//! Getter for Board ID
	auto getDeviceID() const { return m_deviceID; }
	//! Getter for TDC ID
	auto getTDCID() const { return m_tdcID; }

	//! Getter for combined Chain + Device combination
	auto getBoardID() const { return BoardIdentifier(m_chainID, m_deviceID); }

	//! Returns a python-style string with details of the identifier
	std::string str() const;

	//! Operator overload for <
	friend bool operator< (
		const ReadoutIdentifier& lhs, //!< RHS ReadoutIdentifier to be compared to
		const ReadoutIdentifier& rhs //!< LHS ReadoutIdentifier to be compared to
	);

	//! Operator overload for >
	friend bool operator> (
		const ReadoutIdentifier& lhs, //!< RHS ReadoutIdentifier to be compared to
		const ReadoutIdentifier& rhs //!< LHS ReadoutIdentifier to be compared to
	);

	//! Equality operator overload
	friend bool operator==(
		const ReadoutIdentifier& lhs, //!< RHS ReadoutIdentifier to be compared to
		const ReadoutIdentifier& rhs //!< LHS ReadoutIdentifier to be compared to
	);

private:
	const unsigned int m_tdcID;
};

#endif /* READOUTIDENTIFIER_H */
