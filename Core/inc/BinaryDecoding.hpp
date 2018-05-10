#ifndef BINARYDECODING_H
#define BINARYDECODING_H

// LOCAL
#include "Debug.hpp"

namespace bindec {

// Useful alias
using uint = unsigned int;

//! Returns the datatype of the word
inline uint getDataType(
	const uint word
) {
	return word >> 28;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//! Returns the ROC value of the word
inline uint getROCValue(
	const uint word
) {
	ASSERT(15 == getDataType(word));
	return word << 4 >> 4;
}

}; // namespace bindec

#endif /* BINARYDECODING_H */
