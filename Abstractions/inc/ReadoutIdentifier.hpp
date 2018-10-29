#ifndef READOUTIDENTIFIER_H
#define READOUTIDENTIFIER_H

// STD
#include <iostream>
#include <string>

// LOCAL
#include "BoardIdentifier.hpp"

//! Class used to uniquely idenfity combinations of readout informtion
/*!
	Twins information about a Board with a specific TDC ID, uniquely idenfitying a TDC in the electronics.

	Access to board information is provided exclusively through the BoardIdentifier class to prevent modifications to ReadoutIdentifier being required should the board identification system change.
*/
class ReadoutIdentifier {
public:
	//! Constructor from Board ID
	ReadoutIdentifier(
		const BoardIdentifier& boardID, //!< Board ID of the readout
		const unsigned int tdcID //!< TDC ID of the readout
	);

	//! Getter for TDC ID
	auto getTDCID() const { return m_tdcID; }

	//! Getter for combined Chain + Device combination
	auto getBoardID() const { return m_boardID; }

	//! Operator overload for <
	friend bool operator< (
		const ReadoutIdentifier& lhs, //!< RHS ReadoutIdentifier to be compared to
		const ReadoutIdentifier& rhs //!< LHS ReadoutIdentifier to be compared to
	);

	//! Equality operator overload
	friend bool operator==(
		const ReadoutIdentifier& lhs, //!< RHS ReadoutIdentifier to be compared to
		const ReadoutIdentifier& rhs //!< LHS ReadoutIdentifier to be compared to
	);

	//! Overload << operator
	friend std::ostream& operator<<(
		std::ostream& os, //!< Output stream
		const ReadoutIdentifier& identifier //!< ReadoutIdentifier to be output
	);

private:
	const BoardIdentifier m_boardID; //!< Board Identifier
	const unsigned int m_tdcID; //!< Additional TDC information
};

#endif /* READOUTIDENTIFIER_H */
