#ifndef POLARITYMODIFIERS_H
#define POLARITYMODIFIERS_H

// STD
#include <functional>

namespace PolMod {

using uint = unsigned int;

//! Don't apply any changes
std::function<void(uint&)> noChange = [] (
	uint& word
) -> void { };

//! Even channel edge flip
std::function<void(uint&)> flipEven = [] (
	uint& word
) -> void {
	const auto dataType = bindec::getDataType(word);
	ASSERT(4 == dataType || 5 == dataType);

	// If channel is even
	const auto channelID = bindec::getChannelID(word);
	if (0 == channelID%2) {
		// Flip polarity
		if (dataType == 4) {
			word += 0x10000000;
		} else {
			word -= 0x10000000;
		}
	}
};

};

#endif /* POLARITYMODIFIERS_H */
