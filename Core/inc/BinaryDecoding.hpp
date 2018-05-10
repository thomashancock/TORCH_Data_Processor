#ifndef BINARYDECODING_H
#define BINARYDECODING_H

// STD
#include <iostream>

// LOCAL
#include "Debug.hpp"

namespace bindec {

// Useful alias
using uint = unsigned int;

//! Extracts the datatype of the word
inline uint getDataType(
	const uint word
) {
	return word >> 28;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Extracts the ROC value from the word (for data type 15)
inline uint getROCValue(
	const uint word
) {
	ASSERT(15 == getDataType(word));
	return word << 4 >> 4;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Extracts the TDC ID from the word (for data types 2, 3, 4 & 5)
inline uint getTDCID(
	const uint word
) {
	ASSERT(2 <= getDataType(word));
	ASSERT(5 >= getDataType(word));
	return word << 4 >> 28;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Extracts the Channel ID from the word (for data types 4 & 5)
inline uint getChannelID(
	const uint word
) {
	ASSERT(4 == getDataType(word) || 5 == getDataType(word));
	return word << 8 >> 27;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Extracts the Timestamp from the word (for data types 4 & 5)
inline uint getTimestamp(
	const uint word
) {
	ASSERT(4 == getDataType(word) || 5 == getDataType(word));
	return word << 13 >> 13;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Extracts the Event ID from the word (for data types 2 & 3)
inline uint getEventID(
	const uint word
) {
	ASSERT(2 == getDataType(word) || 3 == getDataType(word));
	return word << 8 >> 20;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Extracts the Bunch ID from the word (for data type 2)
inline uint getBunchID(
	const uint word
) {
	ASSERT(2 == getDataType(word));
	return word << 20 >> 20;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Extracts the Bunch ID from the word (for data type 3)
inline uint getWordCount(
	const uint word
) {
	ASSERT(3 == getDataType(word));
	return word << 20 >> 20;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Prints a ROC word in human-readable format (for data type 15)
inline void printROCWord(
	const uint word
) {
	ASSERT(15 == getDataType(word));
	std::cout << "ROC:     " << getROCValue(word) << std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Prints an edge word in human-readable format (for data types 4 & 5)
inline void printEdgeWord(
	const uint word
) {
	ASSERT(4 == getDataType(word) || 5 == getDataType(word));
	std::cout << "EDGE:    "
		<< " TDC: " << getTDCID(word)
		<< " Chl: " << getChannelID(word)
		<< " Time: " << getTimestamp(word)
		<< std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Prints a header word in human-readable format (for data type 2)
inline void printHeaderWord(
	const uint word
) {
	ASSERT(2 == getDataType(word));
	std::cout << "HEADER:  "
		<< " TDC: " << getTDCID(word)
		<< " Evt: " << getEventID(word)
		<< " Bch: " << getBunchID(word)
		<< std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Prints a trailer word in human-readable format (for data type 3)
inline void printTrailerWord(
	const uint word
) {
	ASSERT(3 == getDataType(word));
	std::cout << "TRAILER: "
		<< " TDC: " << getTDCID(word)
		<< " Evt: " << getEventID(word)
		<< " WdC: " << getWordCount(word)
		<< std::endl;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Prints a word in human-readable format (for data types 2, 3, 4, 5 & 15)
inline void printWord(
	const uint word
) {
	const auto dataType = getDataType(word);
	ASSERT(2 == dataType || 3 == dataType || 4 == dataType || 5 == dataType || 15 == dataType);
	if (2 == dataType) {
		printHeaderWord(word);
	} else if (3 == dataType) {
		printTrailerWord(word);
	} else if (15 == dataType) {
		printROCWord(word);
	} else {
		printEdgeWord(word);
	}
}

}; // namespace bindec

#endif /* BINARYDECODING_H */
