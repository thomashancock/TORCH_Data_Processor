#ifndef WORDBUNDLE_H
#define WORDBUNDLE_H

// STD
#include <queue>

class WordBundle {
public:
	WordBundle();

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
	bool m_rocValueStored = false;

	std::queue<unsigned int> m_words;
	unsigned int m_rocValue = 0;
};

#endif /* WORDBUNDLE_H */
