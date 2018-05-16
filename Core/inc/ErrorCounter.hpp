#ifndef ERRORCOUNTER_H
#define ERRORCOUNTER_H

// STD
#include <map>
#include <utility>

//! Used by ErrorSpy to track which readout board/tdc combinations errors have occured on
class ErrorCounter {
	using uintPair = std::pair<unsigned int, unsigned int>;
public:
	//! Constructor
	ErrorCounter();

	//! Incrememnts the count for the readout board/tdc combination
	inline void addCount(
		const unsigned int readoutBoardID, //!< The Readout Board ID
		const unsigned int tdcID //!< The TDC ID
	);

	//! Returns the count for the readout board/tdc combination
	inline unsigned int getCount(
		const unsigned int readoutBoardID, //!< The Readout Board ID
		const unsigned int tdcID //!< The TDC ID
	) const;

	//! Prints the counts stored
	void print() const;

private:
	//! Returns a map key for the given readout board/tdc combination
	/*!
		Returns auto to facilitate easy modifiaction of map key type
	 */
	inline auto makeKey(
		const unsigned int readoutBoardID,
		const unsigned int tdcID
	) const;

private:
	std::map<uintPair, unsigned int> m_counts; //!< Map to store counts in
};

#endif /* ERRORCOUNTER_H */

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline auto ErrorCounter::makeKey(
	const unsigned int readoutBoardID,
	const unsigned int tdcID
) const {
	// Key is currently std::pair, but can be changed
	return std::make_pair(readoutBoardID,tdcID);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline void ErrorCounter::addCount(
	const unsigned int readoutBoardID,
	const unsigned int tdcID
) {
	const auto key = makeKey(readoutBoardID,tdcID);

	auto found = m_counts.find(key);
	if (m_counts.end() == found) {
		m_counts.insert(std::make_pair(key,1));
	} else {
		found->second++;
	}
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline unsigned int ErrorCounter::getCount(
	const unsigned int readoutBoardID,
	const unsigned int tdcID
) const {
	const auto key = makeKey(readoutBoardID,tdcID);

	const auto found = m_counts.find(key);
	if (m_counts.end() == found) {
		return 0;
	} else {
		return found->second;
	}
}
