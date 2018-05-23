#ifndef POLARITYMODIFIERS_H
#define POLARITYMODIFIERS_H

// STD
#include <functional>

// LOCAL
#include "Packet.hpp"

namespace PolMod {

using uint = unsigned int;

//! Don't apply any changes
std::function<void(uint&)> noChange = [] (
	uint& word
) -> void { };
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setPolarityModifier(
	const std::string key
) {
	if ("NoChange" == key) {
		Packet::setPolarityModifier(PolMod::noChange);
	} else if ("FlipEven" == key) {
		Packet::setPolarityModifier(PolMod::flipEven);
	} else {
		STD_ERR("Unrecognised Polarity Modifier Key: " << key);
		exit(0);
	}
}

};

#endif /* POLARITYMODIFIERS_H */
