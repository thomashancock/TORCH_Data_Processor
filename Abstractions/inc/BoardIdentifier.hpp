#ifndef BOARDIDENTIFIER_H
#define BOARDIDENTIFIER_H

// STD
#include <iostream>
#include <string>
#include <array>

//! Class used to uniquely idenfity combinations of board informtion
/*!

*/
class BoardIdentifier {
public:
	BoardIdentifier(
		const unsigned int chainID,
		const unsigned int deviceID
	);

	//! Getter for Chain ID
	auto getChainID() const { return m_chainID; }
	//! Getter for Board ID
	auto getDeviceID() const { return m_deviceID; }
	// //! Getter for TDC ID
	// auto getTDCID() const { return m_tdcID; }
	//
	// //! Getter for combined Chain + Device combination
	// auto getBoardID() const { return std::array<unsigned int, 2> {{ m_chainID, m_deviceID }}; }

	//! Returns a python-style string with details of the identifier
	// std::string str() const;

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
