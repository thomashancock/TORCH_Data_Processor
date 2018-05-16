#ifndef ERRORCOUNTER_H
#define ERRORCOUNTER_H

// STD
#include <map>
#include <utility>

class ErrorCounter {
	using uintPair = std::pair<unsigned int, unsigned int>;
public:
	ErrorCounter();

	inline void addCount(
		const unsigned int readoutBoardID,
		const unsigned int tdcID
	);

	inline unsigned int getCount(
		const unsigned int readoutBoardID,
		const unsigned int tdcID
	);

	void print() const;

private:
	inline auto makeKey(
		const unsigned int readoutBoardID,
		const unsigned int tdcID
	);

private:
	std::map<uintPair, unsigned int> m_counts;
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
) {
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
) {
	const auto key = makeKey(readoutBoardID,tdcID);

	const auto found = m_counts.find(key);
	if (m_counts.end() == found) {
		return 0;
	} else {
		return found->second;
	}
}
