#ifndef BOARDIDENTIFIER_H
#define BOARDIDENTIFIER_H

// STD
#include <iostream>
#include <string>

//! Class used to uniquely idenfity combinations of board informtion
/*!
	Boards are uniquely identified by a combination of Chain ID and Device ID.

	BoardIdentifier stores a combination of these pieces of information and allows comparison to check whether they are the same.
*/
class BoardIdentifier {
public:
	BoardIdentifier(
		const unsigned int chainID, //!< Chain ID of the board
		const unsigned int deviceID //!< Device ID of the board
	);

	//! Getter for Chain ID
	auto getChainID() const { return m_chainID; }

	//! Getter for Board ID
	auto getDeviceID() const { return m_deviceID; }

	//! Operator overload for <
	friend bool operator< (
		const BoardIdentifier& lhs, //!< RHS BoardIdentifier to be compared to
		const BoardIdentifier& rhs //!< LHS BoardIdentifier to be compared to
	);

	//! Operator overload for >
	friend bool operator> (
		const BoardIdentifier& lhs, //!< RHS BoardIdentifier to be compared to
		const BoardIdentifier& rhs //!< LHS BoardIdentifier to be compared to
	);

	//! Equality operator overload
	friend bool operator==(
		const BoardIdentifier& lhs, //!< RHS BoardIdentifier to be compared to
		const BoardIdentifier& rhs //!< LHS BoardIdentifier to be compared to
	);

	//! Overload << operator
	friend std::ostream& operator<<(
		std::ostream& os, //!< Output stream
		const BoardIdentifier& identifier //!< BoardIdentifier to be output
	);

protected:
	const unsigned int m_chainID;
	const unsigned int m_deviceID;
};

#endif /* BOARDIDENTIFIER_H */
