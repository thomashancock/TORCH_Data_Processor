#ifndef ERRORCOUNTER_H
#define ERRORCOUNTER_H

// STD
#include <map>
#include <array>
#include <iostream>

// LOCAL
#include "Debug.hpp"

//! Used by ErrorSpy to track which readout combinations of board/tdc ids that errors have occured on
/*!
		Template class.

		Template parameter N is the dimensionality of the ErrorCounts to be stored
		i.e. For errors specific to a board/tdc id combination, N = 2. For generic errors, N = 0.

		N is given as a parameter to facilitate easier modifications to the ErrorSpy in the future.
 */
template <unsigned int N>
class ErrorCounter {
public:
	//! Constructor
	/*!
		Type checking will ensure Args is a set of std::strings.
	 */
	template <typename... Args>
	ErrorCounter(
		Args... args //!< Parameter pack for headings of each dimension (usually "BoardID" or "TDC")
	);

	//! Incrememnts the count for the specific combination of arguments given
	/*!
		Type checking will ensure Args is a set of unsigned ints.
	 */
	template <typename... Args>
	void addCount(
		Args... args //!< Parameter pack for board/tdc ID combination
	);

	//! Prints the counts stored
	void print() const;

private:
	//! Returns a map key for the given readout board/tdc combination
	/*!
		Type checking will ensure Args is a set of unsigned ints.

		Returns auto to facilitate easy modifiaction of map key type.
	 */
	template <typename... Args>
	auto makeKey(
		Args... args //!< Parameter pack for board/tdc ID combination
	) const;

private:
	const std::array<std::string, N> m_labels;
	std::map< std::array<unsigned int, N> , unsigned int > m_counts; //!< Map to store counts in
};

//! Class specialisation of ErrorCounter for N = 0
template <>
class ErrorCounter<0> {
public:
	//! Constructor
	/*!
		No longer requires arguments as there aren't any dimensions to label.
	 */
	ErrorCounter() { };

	//! Incrememnts the count
	void addCount() { m_count++; };

	//! Prints the stored count
	void print() const {
		std::cout << "\t (x " << m_count << ")" << std::endl;
	};

private:
	unsigned int m_count = 0; //!< Stores the count
};

// Include function implementations
#include "ErrorCounter.inl"

#endif /* ERRORCOUNTER_H */
