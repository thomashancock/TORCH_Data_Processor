#ifndef WORDBUNDLE_H
#define WORDBUNDLE_H

// STD
#include <queue>

// LOCAL
#include "Debug.hpp"

class WordBundle {
public:
	//! Constructor
	WordBundle(
		const unsigned int readoutBoardNumber,
		const char slot
	);

	//! Check if Bundle is complete
	bool isComplete() const { return m_rocValueStored; }

	//! Is bundle empty
	bool empty() const { return m_words.empty(); }

	//! Adds a word to the bundle
	void addWord(
		const unsigned int word
	);

	//! Returns the next word in the bundle
	unsigned int getNextWord();

	//! Returns the stored ROC value
	unsigned int getROCValue() const;

	//! Sets the ROC Value
	void setRocValue(
		const unsigned int rocValue
	);

private:
	const unsigned int m_readoutBoardNumber;
	const char m_slot;

	bool m_rocValueStored = false;

	std::queue<unsigned int> m_words;
	unsigned int m_rocValue = 0;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Inlines:
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline void WordBundle::addWord(
	const unsigned int word
) {
	m_words.push(word);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline unsigned int WordBundle::getNextWord() {
	ASSERT(!m_words.empty());
	const auto word = m_words.front();
	m_words.pop();
	return word;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline unsigned int WordBundle::getROCValue() const {
	ASSERT(true == m_rocValueStored);
	return m_rocValue;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline void WordBundle::setRocValue(
	const unsigned int rocValue
) {
	ASSERT(false == m_rocValueStored);
	m_rocValue = rocValue;
	m_rocValueStored = true;
}

#endif /* WORDBUNDLE_H */
